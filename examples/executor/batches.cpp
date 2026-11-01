#include <wjr/concurrency/scheduler/scoped_executor.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

namespace {
using namespace wjr::sched;

struct checksum_batch {
    const std::vector<uint32_t> *input;
    uint64_t *output;
    size_t next;
    const size_t end;

    task_step operator()(task_context &) noexcept {
        const size_t limit = std::min(next + 128, end);
        for (; next != limit; ++next) {
            *output += uint64_t((*input)[next]) * ((*input)[next] + uint64_t{1});
        }
        return next == end ? task_step::finished : task_step::again;
    }
};
using task_type = cooperative_task<checksum_batch>;

bool submit_batches(const submitter &submit, const std::vector<std::unique_ptr<task_type>> &tasks) {
    constexpr size_t producers = 2;
    std::atomic<bool> failed{false};
    std::array<std::thread, producers> threads;
    for (size_t p = 0; p < producers; ++p) {
        threads[p] = std::thread([&, p] {
            for (size_t i = p; i < tasks.size(); i += producers) {
                if (submit.submit(*tasks[i]) != executor_status::success) {
                    failed.store(true, std::memory_order_relaxed);
                }
            }
        });
    }
    for (auto &thread : threads) {
        thread.join();
    }
    return !failed.load(std::memory_order_relaxed);
}

bool verify(const std::vector<uint32_t> &input,
            const std::vector<std::unique_ptr<task_type>> &tasks,
            const std::array<uint64_t, 16> &outputs) {
    uint64_t actual = 0, expected = 0;
    for (size_t i = 0; i < tasks.size(); ++i) {
        if (tasks[i]->outcome() != task_outcome::completed) {
            return false;
        }
        actual += outputs[i];
    }
    for (auto value : input) {
        expected += uint64_t(value) * (value + uint64_t{1});
    }
    std::cout << "records=" << input.size() << " producers=2 worker=1 checksum=" << actual << '\n';
    return actual == expected;
}
} // namespace

int main() {
    constexpr size_t batches = 16, records = 65536;
    std::vector<uint32_t> input(records);
    for (size_t i = 0; i < records; ++i) {
        input[i] = static_cast<uint32_t>(i % 1009);
    }
    std::array<uint64_t, batches> outputs{};
    std::vector<std::unique_ptr<task_type>> tasks;
    for (size_t i = 0; i < batches; ++i) {
        tasks.push_back(std::make_unique<task_type>(checksum_batch{
            &input, &outputs[i], i * records / batches, (i + 1) * records / batches}));
    }
    cooperative_executor executor;
    task_scope scope(executor, batches);
    const bool accepted = submit_batches(scope.make_submitter(), tasks);
    if (scope.join() != executor_status::success || !accepted) {
        return 1;
    }
    return verify(input, tasks, outputs) ? 0 : 2;
}
