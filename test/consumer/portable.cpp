#include <wjr/ring_buffer.hpp>

int main() {
    wjr::ring_buffer<int> queue;
    queue.push_back(7);
    queue.push_back(11);
    queue.pop_front();
    return queue.front() == 11 && queue.size() == 1 ? 0 : 1;
}
