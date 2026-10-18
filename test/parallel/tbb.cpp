#include <array>
#include <atomic>
#include <limits>
#include <memory>

#include <gtest/gtest.h>
#include <oneapi/tbb/task_arena.h>
#include <oneapi/tbb/task_group.h>

#include <wjr/parallel/tbb.hpp>

namespace {
struct mutable_only {
    void operator()(size_t, size_t) {}
};
struct const_callable {
    int operator()(size_t, size_t) const noexcept { return 42; }
};
template <class F>
concept chunk_callable =
    requires(const F &function) { wjr::tbb::parallel_for_chunks(0, 1, 1, function); };
static_assert(chunk_callable<const_callable>);
static_assert(!chunk_callable<mutable_only>);
static_assert(!noexcept(wjr::tbb::parallel_for_chunks(0, 1, 1, const_callable{})));

TEST(TbbChunks, EmptyDoesNotInvokeAndReturnValueIsIgnored) {
    unsigned calls = 0;
    wjr::tbb::parallel_for_chunks(91, 91, 1, [&](size_t, size_t) { ++calls; });
    EXPECT_EQ(calls, 0u);
    wjr::tbb::parallel_for_chunks(0, 1, 1, const_callable{});
}

TEST(TbbChunks, CoversUnevenAndLargeIndexRangesExactlyOnce) {
    for (const auto first : {size_t{0}, size_t{31}, std::numeric_limits<size_t>::max() - 257})
        for (const auto count : {size_t{1}, size_t{17}, size_t{257}})
            for (const auto grain : {size_t{1}, size_t{7}, size_t{1024}}) {
                std::array<std::atomic<unsigned>, 257> visits{};
                std::atomic<bool> invalid = false;
                wjr::tbb::parallel_for_chunks(
                    first, first + count, grain, [&](size_t begin, size_t end) {
                        if (begin < first || end > first + count || begin >= end) {
                            invalid.store(true, std::memory_order_relaxed);
                            return;
                        }
                        for (auto i = begin; i != end; ++i)
                            visits[i - first].fetch_add(1, std::memory_order_relaxed);
                    });
                EXPECT_FALSE(invalid.load());
                for (size_t i = 0; i != visits.size(); ++i)
                    EXPECT_EQ(visits[i].load(), i < count ? 1u : 0u);
            }
}

struct identity_callable {
    std::atomic<unsigned> &copies;
    std::atomic<unsigned> &moves;
    std::atomic<size_t> &visited;
    std::atomic<bool> &wrong_object;
    const identity_callable *original = this;

    identity_callable(std::atomic<unsigned> &c, std::atomic<unsigned> &m, std::atomic<size_t> &v,
                      std::atomic<bool> &w) noexcept
        : copies(c), moves(m), visited(v), wrong_object(w) {}
    identity_callable(const identity_callable &other) noexcept
        : copies(other.copies), moves(other.moves), visited(other.visited),
          wrong_object(other.wrong_object), original(other.original) {
        ++copies;
    }
    identity_callable(identity_callable &&other) noexcept
        : copies(other.copies), moves(other.moves), visited(other.visited),
          wrong_object(other.wrong_object), original(other.original) {
        ++moves;
    }
    void operator()(size_t begin, size_t end) const noexcept {
        if (this != original)
            wrong_object.store(true, std::memory_order_relaxed);
        visited.fetch_add(end - begin, std::memory_order_relaxed);
    }
};

TEST(TbbChunks, BorrowsOriginalConstCallableWithoutCopiesOrMoves) {
    std::atomic<unsigned> copies = 0, moves = 0;
    std::atomic<size_t> visited = 0;
    std::atomic<bool> wrong = false;
    const identity_callable function(copies, moves, visited, wrong);
    wjr::tbb::parallel_for_chunks(3, 2003, 7, function);
    EXPECT_EQ(visited.load(), 2000u);
    EXPECT_EQ(copies.load(), 0u);
    EXPECT_EQ(moves.load(), 0u);
    EXPECT_FALSE(wrong.load());
}

TEST(TbbChunks, AcceptsMoveOnlyTemporaryConstCallable) {
    std::atomic<size_t> visited = 0;
    wjr::tbb::parallel_for_chunks(
        0, 513, 9, [marker = std::make_unique<int>(3), &visited](size_t begin, size_t end) {
            visited.fetch_add((end - begin) * *marker, std::memory_order_relaxed);
            return true;
        });
    EXPECT_EQ(visited.load(), 1539u);
}

TEST(TbbChunks, ReusesCallerArenaAndSupportsNestedChunks) {
    oneapi::tbb::task_arena arena(2);
    for (unsigned repeat = 0; repeat != 2; ++repeat) {
        std::array<std::atomic<unsigned>, 13 * 17> visits{};
        arena.execute([&] {
            wjr::tbb::parallel_for_chunks(0, 13, 2, [&](size_t begin, size_t end) {
                for (auto row = begin; row != end; ++row)
                    wjr::tbb::parallel_for_chunks(0, 17, 3, [&, row](size_t b, size_t e) {
                        for (auto col = b; col != e; ++col)
                            visits[row * 17 + col].fetch_add(1, std::memory_order_relaxed);
                    });
            });
        });
        for (const auto &visit : visits)
            EXPECT_EQ(visit.load(), 1u);
    }
}

TEST(TbbChunks, InheritsParentCancellationWithoutAChildException) {
    oneapi::tbb::task_group_context context;
    std::atomic<unsigned> calls = 0;
    bool returned = false;
    oneapi::tbb::parallel_for(
        oneapi::tbb::blocked_range<size_t>(0, 1),
        [&](const auto &) {
            context.cancel_group_execution();
            wjr::tbb::parallel_for_chunks(
                0, 1024, 1, [&](size_t, size_t) { calls.fetch_add(1, std::memory_order_relaxed); });
            returned = true;
        },
        oneapi::tbb::auto_partitioner{}, context);
    EXPECT_TRUE(returned);
    EXPECT_EQ(calls.load(), 0u);
}

#ifndef WJR_DISABLE_EXCEPTIONS
struct callback_error {};
struct active_callback {
    std::atomic<unsigned> &count;
    explicit active_callback(std::atomic<unsigned> &value) noexcept : count(value) { ++count; }
    ~active_callback() { --count; }
};

// Keep the scheduler/callback boundary separate from GTest's catch in Windows
// Clang ASan builds; inlining both here can trigger a silent runtime trap.
WJR_NOINLINE void throwing_chunks(std::atomic<unsigned> &active,
                                  std::array<std::atomic<unsigned>, 1024> &written) {
    wjr::tbb::parallel_for_chunks(0, written.size(), 3, [&](size_t begin, size_t end) {
        active_callback scope(active);
        written[begin].store(1, std::memory_order_relaxed);
        if (begin == 0)
            throw callback_error{};
        for (auto i = begin + 1; i != end; ++i)
            written[i].store(1, std::memory_order_relaxed);
    });
}

TEST(TbbChunks, PropagatesCallbackExceptionAfterCallbacksStop) {
    std::atomic<unsigned> active = 0;
    std::array<std::atomic<unsigned>, 1024> written{};
    EXPECT_THROW(throwing_chunks(active, written), callback_error);
    EXPECT_EQ(active.load(), 0u);
    EXPECT_EQ(written[0].load(), 1u);
}
#endif
} // namespace
