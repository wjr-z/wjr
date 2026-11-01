#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <vector>

#include <wjr/concurrency/scheduler/eevdf.hpp>

struct batch : wjr::sched::eevdf_entity {
    const std::vector<uint64_t> *input = nullptr;
    size_t position = 0;
    size_t chunk = 1;
    uint64_t checksum = 0;
};

static size_t process_chunk(batch &job) noexcept {
    const size_t count = std::min(job.chunk, job.input->size() - job.position);
    for (size_t i = 0; i < count; ++i) {
        job.checksum += (*job.input)[job.position++];
    }
    return count;
}

int main() {
    using namespace wjr::sched;
    std::vector<uint64_t> small(4096), large(100000);
    std::iota(small.begin(), small.end(), uint64_t{1});
    std::iota(large.begin(), large.end(), uint64_t{3});
    std::array<batch, 2> work;
    work[0].input = &small;
    work[0].chunk = 32;
    work[1].input = &large;
    work[1].chunk = 256;
    eevdf_queue queue;
    if (queue.enqueue(work[0], {1, 32}) != eevdf_status::success ||
        queue.enqueue(work[1], {3, 256}) != eevdf_status::success) {
        (void)queue.clear();
        return 1;
    }
    queue.close();
    while (!queue.empty()) {
        const auto dispatch = queue.begin_dispatch();
        if (dispatch.status() != eevdf_status::success) {
            (void)queue.clear();
            return 2;
        }
        auto &job = *static_cast<batch *>(dispatch.entity());
        const size_t count = process_chunk(job);
        if (queue.complete_dispatch(count, job.position == job.input->size()) !=
            eevdf_status::success) {
            (void)queue.clear();
            return 3;
        }
    }
    for (const auto &job : work) {
        if (job.checksum != std::accumulate(job.input->begin(), job.input->end(), uint64_t{0})) {
            return 4;
        }
    }
    std::cout << "processed " << small.size() << " and " << large.size()
              << " records; service units are records, not CPU nanoseconds\n";
}
