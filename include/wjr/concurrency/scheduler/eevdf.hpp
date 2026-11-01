#ifndef WJR_CONCURRENCY_SCHEDULER_EEVDF_HPP__
#define WJR_CONCURRENCY_SCHEDULER_EEVDF_HPP__

#include <cstddef>
#include <cstdint>

#include <wjr/container/list.hpp>

namespace wjr::sched {

class eevdf_queue;

enum class eevdf_status {
    success,
    empty,
    busy,
    closed,
    faulted,
    invalid_parameters,
    already_registered,
    wrong_queue,
    capacity_limit,
    no_dispatch,
    invalid_service,
    accounting_limit
};

struct eevdf_parameters {
    uint32_t weight = 1;
    uint32_t request = 1;
};

/**
 * @brief Caller-owned, stable-address scheduling identity with no payload ownership.
 * @pre Detach the entity before destroying it. Inspect accounting fields only while registered.
 */
class eevdf_entity : private intrusive::list_node<eevdf_entity> {
    using hook = intrusive::list_node<eevdf_entity>;
    friend class eevdf_queue;
    friend class intrusive::list_node_base<eevdf_entity>;

public:
    eevdf_entity() noexcept : hook(default_construct) {}
    eevdf_entity(const eevdf_entity &) = delete;
    eevdf_entity(eevdf_entity &&) = delete;
    eevdf_entity &operator=(const eevdf_entity &) = delete;
    eevdf_entity &operator=(eevdf_entity &&) = delete;
    ~eevdf_entity() noexcept { WJR_ASSERT_L2(m_owner == nullptr); }

    bool registered() const noexcept { return m_owner != nullptr; }
    uint32_t weight() const noexcept {
        WJR_ASSERT_L2(registered());
        return m_parameters.weight;
    }
    uint32_t request() const noexcept {
        WJR_ASSERT_L2(registered());
        return m_parameters.request;
    }
    uint64_t virtual_runtime() const noexcept {
        WJR_ASSERT_L2(registered());
        return m_runtime;
    }
    uint64_t virtual_deadline() const noexcept {
        WJR_ASSERT_L2(registered());
        return m_deadline;
    }

private:
    eevdf_queue *m_owner = nullptr;
    eevdf_parameters m_parameters;
    uint32_t m_fraction = 0;
    uint64_t m_runtime = 0;
    uint64_t m_deadline = 0;
};

class eevdf_dispatch {
public:
    constexpr eevdf_dispatch(eevdf_status status, eevdf_entity *entity) noexcept
        : m_status(status), m_entity(entity) {}
    constexpr eevdf_status status() const noexcept { return m_status; }
    constexpr eevdf_entity *entity() const noexcept { return m_entity; }

private:
    eevdf_status m_status;
    eevdf_entity *m_entity;
};

/**
 * @brief Single-owner, bounded ready-only EEVDF queue using explicit service units.
 * @pre No concurrent access. Settle dispatch and detach every entity before destruction.
 * @note No threads, clocks, payload execution, allocation, sleeping or external waits.
 * Selection is linear; deadlines survive early yields. Equal keys use registration order.
 */
class eevdf_queue {
    using hook = intrusive::list_node<eevdf_entity>;

public:
    static constexpr size_t max_entities = 256;
    static constexpr uint32_t max_weight = 4096;
    static constexpr uint32_t max_service = uint32_t{1} << 20;
    static constexpr uint64_t virtual_scale = uint64_t{1} << 16;
    static constexpr uint64_t max_virtual_span = uint64_t{1} << 40;
    static constexpr uint64_t rebase_threshold = uint64_t{1} << 48;

    eevdf_queue() = default;
    eevdf_queue(const eevdf_queue &) = delete;
    eevdf_queue(eevdf_queue &&) = delete;
    eevdf_queue &operator=(const eevdf_queue &) = delete;
    eevdf_queue &operator=(eevdf_queue &&) = delete;
    ~eevdf_queue() noexcept { WJR_ASSERT_L2(empty() && m_current == nullptr); }

    size_t size() const noexcept { return m_size; }
    bool empty() const noexcept { return m_size == 0; }
    bool closed() const noexcept { return m_closed; }
    bool faulted() const noexcept { return m_faulted; }
    eevdf_entity *current() const noexcept { return m_current; }

    /// @brief Register fresh work at the current virtual average; admission may fail without
    /// mutation.
    eevdf_status enqueue(eevdf_entity &entity, eevdf_parameters parameters = {}) noexcept {
        if (m_faulted) {
            return eevdf_status::faulted;
        }
        if (m_closed) {
            return eevdf_status::closed;
        }
        if (m_current != nullptr) {
            return eevdf_status::busy;
        }
        if (entity.registered()) {
            return eevdf_status::already_registered;
        }
        if (!valid_parameters(parameters)) {
            return eevdf_status::invalid_parameters;
        }
        if (m_size == max_entities) {
            return eevdf_status::capacity_limit;
        }

        auto aggregate = summarize();
        rebase(aggregate);
        entity.m_parameters = parameters;
        entity.m_runtime = aggregate.base;
        if (aggregate.weight != 0) {
            entity.m_runtime += aggregate.sum / aggregate.weight;
        }
        entity.m_deadline = entity.m_runtime + virtual_request(entity);
        entity.m_fraction = 0;
        m_head.push_back(&entity);
        entity.m_owner = this;
        ++m_size;
        return eevdf_status::success;
    }

    /// @brief Select the earliest eligible deadline, retaining it in accounting until completion.
    eevdf_dispatch begin_dispatch() noexcept {
        if (m_faulted) {
            return {eevdf_status::faulted, nullptr};
        }
        if (m_current != nullptr) {
            return {eevdf_status::busy, nullptr};
        }
        if (empty()) {
            return {eevdf_status::empty, nullptr};
        }
        auto aggregate = summarize();
        rebase(aggregate);
        m_dispatch_base = aggregate.base;
        for (auto *node = m_head.next(); node != &m_head; node = node->next()) {
            auto *entity = node->self();
            const auto relative = entity->m_runtime - aggregate.base;
            if (relative * aggregate.weight <= aggregate.sum &&
                (m_current == nullptr || entity->m_deadline < m_current->m_deadline)) {
                m_current = entity;
            }
        }
        WJR_ASSERT_L2(m_current != nullptr);
        return {eevdf_status::success, m_current};
    }

    /**
     * @brief Charge performed service, then retain ready work or detach finished work.
     * @note Zero service may finish a job but cannot requeue it. An accounting-limit failure
     * leaves numerical state unchanged and permanently faults/seals this queue. Clear then destroy
     * it after the caller's work has returned; do not silently discard an unaccounted segment.
     */
    eevdf_status complete_dispatch(uint64_t service, bool finished = false) noexcept {
        if (m_faulted) {
            return eevdf_status::faulted;
        }
        if (m_current == nullptr) {
            return eevdf_status::no_dispatch;
        }
        if (service == 0 && !finished) {
            return eevdf_status::invalid_service;
        }
        if (service > max_service) {
            return fail_accounting();
        }
        return account_current(static_cast<uint32_t>(service), finished);
    }

    /// @brief Remove ready work without destroying its payload; a current dispatch must settle
    /// first.
    eevdf_status cancel(eevdf_entity &entity) noexcept {
        if (m_current != nullptr) {
            return eevdf_status::busy;
        }
        if (entity.m_owner != this) {
            return eevdf_status::wrong_queue;
        }
        detach(entity);
        return eevdf_status::success;
    }

    /// @brief Seal admissions; already registered work remains available to drain.
    void close() noexcept { m_closed = true; }

    /// @brief Detach all borrowed work without executing it; also releases a faulted dispatch.
    eevdf_status clear() noexcept {
        if (m_current != nullptr && !m_faulted) {
            return eevdf_status::busy;
        }
        m_current = nullptr;
        while (!m_head.empty()) {
            auto *entity = m_head.pop_front()->self();
            entity->m_owner = nullptr;
        }
        m_size = 0;
        return eevdf_status::success;
    }

private:
    static bool valid_parameters(eevdf_parameters parameters) noexcept {
        return parameters.weight != 0 && parameters.weight <= max_weight &&
               parameters.request != 0 && parameters.request <= max_service;
    }

    /// @brief Commit a validated charge only after the complete numerical candidate is in range.
    eevdf_status account_current(uint32_t service, bool finished) noexcept {
        auto &entity = *m_current;
        const auto numerator = service * virtual_scale + entity.m_fraction;
        const auto runtime = entity.m_runtime + numerator / entity.m_parameters.weight;
        if (runtime - m_dispatch_base > max_virtual_span) {
            return fail_accounting();
        }
        entity.m_runtime = runtime;
        entity.m_fraction = static_cast<uint32_t>(numerator % entity.m_parameters.weight);
        if (runtime >= entity.m_deadline) {
            entity.m_deadline = runtime + virtual_request(entity);
        }
        m_current = nullptr;
        if (finished) {
            detach(entity);
        }
        return eevdf_status::success;
    }

    struct aggregate {
        uint64_t base = 0;
        uint64_t sum = 0;
        uint64_t weight = 0;
    };

    aggregate summarize() const noexcept {
        aggregate result;
        for (auto *node = m_head.next(); node != &m_head; node = node->next()) {
            const auto *entity = node->self();
            const auto runtime = entity->m_runtime;
            if (result.weight == 0) {
                result.base = runtime;
            } else if (runtime < result.base) {
                result.sum += (result.base - runtime) * result.weight;
                result.base = runtime;
            }
            result.sum += (runtime - result.base) * entity->m_parameters.weight;
            result.weight += entity->m_parameters.weight;
        }
        return result;
    }

    /// @brief Common-origin subtraction preserves all differences, deadlines and registration ties.
    void rebase(aggregate &aggregate) noexcept {
        if (aggregate.base < rebase_threshold) {
            return;
        }
        for (auto *node = m_head.next(); node != &m_head; node = node->next()) {
            auto *entity = node->self();
            entity->m_runtime -= aggregate.base;
            entity->m_deadline -= aggregate.base;
        }
        aggregate.base = 0;
    }

    static uint64_t virtual_request(const eevdf_entity &entity) noexcept {
        const auto weight = entity.m_parameters.weight;
        return (uint64_t{entity.m_parameters.request} * virtual_scale + weight - 1) / weight;
    }

    void detach(eevdf_entity &entity) noexcept {
        entity.remove();
        entity.m_owner = nullptr;
        --m_size;
    }

    eevdf_status fail_accounting() noexcept {
        m_faulted = true;
        m_closed = true;
        return eevdf_status::accounting_limit;
    }

    hook m_head;
    eevdf_entity *m_current = nullptr;
    uint64_t m_dispatch_base = 0;
    size_t m_size = 0;
    bool m_closed = false;
    bool m_faulted = false;
};

} // namespace wjr::sched

#endif // WJR_CONCURRENCY_SCHEDULER_EEVDF_HPP__
