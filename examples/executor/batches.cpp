#include <wjr/concurrency/scheduler/scoped_executor.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
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
constexpr size_t batch_count = 16;

template <size_t... I>
auto make_tasks(const std::vector<uint32_t> &input, std::array<uint64_t, batch_count> &outputs,
                std::index_sequence<I...>) {
    return std::array<task_type, batch_count>{
        cooperative_task(checksum_batch{&input, &outputs[I], I * input.size() / batch_count,
                                        (I + 1) * input.size() / batch_count})...};
}

bool submit_batches(const submitter &submit, std::array<task_type, batch_count> &tasks) {
    constexpr size_t producers = 2;
    std::atomic<bool> failed{false};
    std::array<std::thread, producers> threads;
    for (size_t p = 0; p < producers; ++p) {
        threads[p] = std::thread([&, p] {
            constexpr size_t per_producer = batch_count / producers;
            if (submit.submit_batch(std::span(tasks).subspan(p * per_producer, per_producer)) !=
                executor_status::success) {
                failed.store(true, std::memory_order_relaxed);
            }
        });
    }
    for (auto &thread : threads) {
        thread.join();
    }
    return !failed.load(std::memory_order_relaxed);
}

bool verify(const std::vector<uint32_t> &input, const std::array<task_type, batch_count> &tasks,
            const std::array<uint64_t, batch_count> &outputs) {
    uint64_t actual = 0, expected = 0;
    for (size_t i = 0; i < tasks.size(); ++i) {
        if (tasks[i].outcome() != task_outcome::completed) {
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
    constexpr size_t records = 65536;
    std::vector<uint32_t> input(records);
    for (size_t i = 0; i < records; ++i) {
        input[i] = static_cast<uint32_t>(i % 1009);
    }
    std::array<uint64_t, batch_count> outputs{};
    auto tasks = make_tasks(input, outputs, std::make_index_sequence<batch_count>{});
    cooperative_executor executor;
    task_scope scope(executor, batch_count);
    const bool accepted = submit_batches(scope.make_submitter(), tasks);
    if (scope.join() != executor_status::success || !accepted) {
        return 1;
    }
    return verify(input, tasks, outputs) ? 0 : 2;
}
