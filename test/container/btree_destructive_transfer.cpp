#include <wjr/container/btree_map.hpp>
#include <wjr/container/btree_set.hpp>
#include <wjr/container/experimental/compact_btree_map.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <map>
#include <memory_resource>
#include <string>

#include <gtest/gtest.h>

namespace {
struct transfer_ledger {
    size_t live = 0;
    size_t peak = 0;
    std::function<void()> observe;
};

class transfer_resource final : public std::pmr::memory_resource {
public:
    explicit transfer_resource(transfer_ledger &ledger) : ledger(ledger) {}
    size_t live = 0;
    size_t allocations = 0;
    size_t deallocations = 0;

private:
    void *do_allocate(size_t bytes, size_t alignment) override {
        void *ptr = std::pmr::new_delete_resource()->allocate(bytes, alignment);
        blocks.emplace(ptr, std::pair{bytes, alignment});
        live += bytes;
        ledger.live += bytes;
        ledger.peak = std::max(ledger.peak, ledger.live);
        ++allocations;
        if (ledger.observe)
            ledger.observe();
        return ptr;
    }
    void do_deallocate(void *ptr, size_t bytes, size_t alignment) override {
        const auto entry = blocks.find(ptr);
        ASSERT_NE(entry, blocks.end());
        EXPECT_EQ(entry->second, (std::pair{bytes, alignment}));
        blocks.erase(entry);
        if (ledger.observe)
            ledger.observe();
        live -= bytes;
        ledger.live -= bytes;
        ++deallocations;
        std::pmr::new_delete_resource()->deallocate(ptr, bytes, alignment);
    }
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override {
        return this == &other;
    }
    transfer_ledger &ledger;
    std::map<void *, std::pair<size_t, size_t>> blocks;
};

struct alignas(64) transfer_value {
    static inline int live = 0;
    static inline std::function<void()> observe;
    int value;
    const transfer_value *self = this;
    explicit transfer_value(int value) noexcept : value(value) { ++live; }
    transfer_value(const transfer_value &) = delete;
    transfer_value(transfer_value &&other) noexcept : value(other.value) {
        EXPECT_EQ(other.self, &other);
        ++live;
        if (observe)
            observe();
    }
    ~transfer_value() noexcept {
        EXPECT_EQ(self, this);
        if (observe)
            observe();
        --live;
    }
};

using transfer_allocator = std::pmr::polymorphic_allocator<char>;

template <typename Tree>
void observe_migration() {
    for (int count : {0, 1, 2, 3, 80}) {
        transfer_ledger ledger;
        transfer_resource first(ledger), second(ledger);
        {
            Tree source(std::less<>{}, transfer_allocator(&first));
            Tree target(std::less<>{}, transfer_allocator(&second));
            for (int i = 0; i < count; ++i)
                source.try_emplace(i, i);
            auto observe = [&] {
                EXPECT_TRUE(source.empty());
                EXPECT_EQ(source.begin(), source.end());
                EXPECT_EQ(source.lower_bound(0), source.end());
                EXPECT_TRUE(target.empty());
                EXPECT_EQ(target.begin(), target.end());
                EXPECT_EQ(target.lower_bound(0), target.end());
            };
            ledger.observe = observe;
            transfer_value::observe = observe;
            target = std::move(source);
            ledger.observe = {};
            transfer_value::observe = {};
            EXPECT_TRUE(source.empty());
            EXPECT_EQ(target.size(), static_cast<size_t>(count));
            EXPECT_EQ(first.live, 0u);
            for (const auto &[key, value] : target) {
                EXPECT_EQ(value.value, key);
                EXPECT_EQ(value.self, &value);
            }
            source.try_emplace(123, 123);
            EXPECT_EQ(source.begin()->first, 123);
        }
        EXPECT_EQ(ledger.live, 0u);
        EXPECT_EQ(first.allocations, first.deallocations);
        EXPECT_EQ(second.allocations, second.deallocations);
        EXPECT_EQ(transfer_value::live, 0);
    }
}

TEST(BtreeDestructiveTransfer, HooksObserveValidEmptySourceAndDestination) {
    observe_migration<wjr::experimental::compact_btree_map<int, transfer_value, std::less<>,
                                                           transfer_allocator, 16>>();
}

template <typename Tree, bool Reclaim>
void check_peak() {
    transfer_ledger ledger;
    transfer_resource first(ledger), second(ledger);
    {
        Tree source(std::less<>{}, transfer_allocator(&first));
        for (uint64_t i = 0; i != 4096; ++i)
            source.try_emplace((i * 7919) % 4096);
        const auto original = ledger.live;
        ledger.peak = original;
        Tree target(std::move(source), transfer_allocator(&second));
        if constexpr (Reclaim) {
            EXPECT_LT(ledger.peak, original + original / 10);
        } else {
            EXPECT_EQ(ledger.peak, 2 * original);
        }
        EXPECT_EQ(first.live, 0u);
        EXPECT_TRUE(source.empty());
        EXPECT_EQ(target.size(), 4096u);
        EXPECT_TRUE(std::is_sorted(target.begin(), target.end(), target.value_comp()));
        EXPECT_EQ(first.allocations, first.deallocations);
    }
    EXPECT_EQ(ledger.live, 0u);
    EXPECT_EQ(second.allocations, second.deallocations);
}

TEST(BtreeDestructiveTransfer, PeakRequestsStayCloseToOneTree) {
    using large = std::array<std::byte, 64>;
    check_peak<wjr::btree_map<uint64_t, uint64_t, std::less<>, transfer_allocator, 16>, false>();
    check_peak<wjr::btree_map<uint64_t, large, std::less<>, transfer_allocator, 16>, false>();
    check_peak<wjr::experimental::compact_btree_map<uint64_t, uint64_t, std::less<>,
                                                    transfer_allocator, 16>,
               true>();
    check_peak<
        wjr::experimental::compact_btree_map<uint64_t, large, std::less<>, transfer_allocator, 16>,
        true>();
}
} // namespace
