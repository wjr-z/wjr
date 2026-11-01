#include "../detail.hpp"

#include <algorithm>
#include <list>
#include <random>
#include <utility>

#include <wjr/container/lru_cache.hpp>

namespace {
class model_trial {
public:
    explicit model_trial(size_t capacity) : cache(capacity) {}

    void step(unsigned operation, int key, int value, size_t index) {
        switch (operation) {
        case 0:
            insert(key, value, false);
            break;
        case 1:
            insert(key, value, true);
            break;
        case 2:
            get(key);
            break;
        case 3:
            peek(key);
            break;
        case 4:
            erase(key);
            break;
        case 5:
            if (index % 127 == 0) {
                cache.clear();
                model.clear();
            }
            break;
        }
        validate();
    }

    void insert(int key, int value, bool assign) {
        if (assign)
            cache.insert_or_assign(key, value);
        else
            cache.try_emplace(key, value);
        admit(key, value, assign);
    }

    void validate() const {
        EXPECT_EQ(cache.size(), model.size());
        EXPECT_LE(cache.size(), cache.capacity());
        for (int probe = 0; probe < 97; ++probe)
            peek(probe);
        for (const auto &[key, value] : model) {
            ASSERT_TRUE(cache.peek(key));
            EXPECT_EQ(*cache.peek(key), value);
        }
    }

private:
    auto locate(int key) const {
        return std::find_if(model.begin(), model.end(),
                            [key](const auto &value) { return value.first == key; });
    }

    auto locate(int key) {
        return std::find_if(model.begin(), model.end(),
                            [key](const auto &value) { return value.first == key; });
    }

    void admit(int key, int value, bool assign) {
        if (cache.capacity() == 0)
            return;
        auto position = locate(key);
        if (position != model.end()) {
            if (assign)
                position->second = value;
            model.splice(model.begin(), model, position);
        } else {
            model.emplace_front(key, value);
            if (model.size() > cache.capacity())
                model.pop_back();
        }
    }

    void get(int key) {
        auto position = locate(key);
        auto *found = cache.get(key);
        EXPECT_EQ(found != nullptr, position != model.end());
        if (found) {
            ASSERT_NE(position, model.end());
            EXPECT_EQ(*found, position->second);
            model.splice(model.begin(), model, position);
        }
    }

    void peek(int key) const {
        auto position = locate(key);
        const auto *found = cache.peek(key);
        EXPECT_EQ(found != nullptr, position != model.end());
        if (found) {
            ASSERT_NE(position, model.end());
            EXPECT_EQ(*found, position->second);
        }
    }

    void erase(int key) {
        auto position = locate(key);
        EXPECT_EQ(cache.erase(key), position != model.end());
        if (position != model.end())
            model.erase(position);
    }

    wjr::lru_cache<int, int> cache;
    std::list<std::pair<int, int>> model;
};
} // namespace

TEST(lru_cache, randomized_model_including_recency_eviction) {
    std::mt19937 random(731);
    for (size_t capacity : {0, 1, 2, 7, 64}) {
        model_trial trial(capacity);
        for (size_t step = 0; step < 4000; ++step) {
            const int key = static_cast<int>(random() % 97), value = static_cast<int>(random());
            trial.step(random() % 6, key, value, step);
        }
        for (int key = 100; key < 100 + static_cast<int>(capacity); ++key) {
            trial.insert(key, key, false);
            trial.validate();
        }
    }
}
