#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <random>
#include <type_traits>

#include <wjr/concurrency/scheduler/eevdf.hpp>

namespace {
using namespace wjr::sched;

template <size_t N>
struct jobs {
    std::array<eevdf_entity, N> entities;
    eevdf_queue queue;
    ~jobs() {
        if (queue.current() != nullptr && !queue.faulted()) {
            (void)queue.complete_dispatch(0, true);
        }
        (void)queue.clear();
    }
};

static_assert(!std::is_copy_constructible_v<eevdf_entity>);
static_assert(!std::is_move_constructible_v<eevdf_entity>);
static_assert(!std::is_copy_constructible_v<eevdf_queue>);
static_assert(noexcept(std::declval<eevdf_queue &>().begin_dispatch()));
static_assert(eevdf_queue::max_virtual_span * eevdf_queue::max_entities * eevdf_queue::max_weight <
              (uint64_t{1} << 63));

/// @brief Independent small-case model: pairwise signed lag, with cumulative service conversion.
struct reference_entity {
    uint64_t runtime = 0;
    uint64_t deadline = 0;
    uint64_t service = 0;
    uint32_t weight;
    uint32_t request;
};

template <size_t N>
size_t reference_pick(const std::array<reference_entity, N> &entities) {
    size_t best = N;
    for (size_t i = 0; i < N; ++i) {
        int64_t lag = 0;
        for (const auto &other : entities) {
            lag +=
                (static_cast<int64_t>(other.runtime) - static_cast<int64_t>(entities[i].runtime)) *
                other.weight;
        }
        if (lag >= 0 && (best == N || entities[i].deadline < entities[best].deadline)) {
            best = i;
        }
    }
    return best;
}

void reference_charge(reference_entity &entity, uint32_t service) {
    entity.service += service;
    entity.runtime = entity.service * eevdf_queue::virtual_scale / entity.weight;
    if (entity.runtime >= entity.deadline) {
        entity.deadline = entity.runtime + (uint64_t{entity.request} * eevdf_queue::virtual_scale +
                                            entity.weight - 1) /
                                               entity.weight;
    }
}
} // namespace

TEST(eevdf, eligibility_precedes_deadline_and_yield_keeps_request) {
    jobs<2> state;
    auto &first = state.entities[0];
    auto &second = state.entities[1];
    ASSERT_EQ(state.queue.enqueue(first, {1, 10}), eevdf_status::success);
    ASSERT_EQ(state.queue.enqueue(second, {1, 1}), eevdf_status::success);
    EXPECT_EQ(state.queue.begin_dispatch().entity(), &second);
    EXPECT_EQ(state.queue.complete_dispatch(1), eevdf_status::success);
    EXPECT_LT(second.virtual_deadline(), first.virtual_deadline());
    EXPECT_EQ(state.queue.begin_dispatch().entity(), &first);
    const auto deadline = first.virtual_deadline();
    EXPECT_EQ(state.queue.complete_dispatch(1), eevdf_status::success);
    EXPECT_EQ(first.virtual_deadline(), deadline);
    EXPECT_EQ(state.queue.begin_dispatch().entity(), &second);
    EXPECT_EQ(state.queue.complete_dispatch(1, true), eevdf_status::success);
    EXPECT_FALSE(second.registered());
    EXPECT_EQ(state.queue.size(), 1u);
}

TEST(eevdf, stable_ties_and_full_overrun_charge) {
    jobs<2> state;
    for (auto &entity : state.entities) {
        ASSERT_EQ(state.queue.enqueue(entity, {1, 10}), eevdf_status::success);
    }
    EXPECT_EQ(state.queue.begin_dispatch().entity(), &state.entities[0]);
    EXPECT_EQ(state.queue.complete_dispatch(21), eevdf_status::success);
    EXPECT_EQ(state.entities[0].virtual_runtime(), 21 * eevdf_queue::virtual_scale);
    EXPECT_EQ(state.entities[0].virtual_deadline(), 31 * eevdf_queue::virtual_scale);
    EXPECT_EQ(state.queue.begin_dispatch().entity(), &state.entities[1]);
    EXPECT_EQ(state.queue.complete_dispatch(21), eevdf_status::success);
    EXPECT_EQ(state.queue.begin_dispatch().entity(), &state.entities[0]);
}

TEST(eevdf, exact_fractional_accounting_matches_independent_integer_model) {
    jobs<3> state;
    std::array<reference_entity, 3> reference = {
        reference_entity{0, 0, 0, 3, 1}, {0, 0, 0, 7, 5}, {0, 0, 0, 11, 9}};
    for (size_t i = 0; i < reference.size(); ++i) {
        const auto &item = reference[i];
        ASSERT_EQ(state.queue.enqueue(state.entities[i], {item.weight, item.request}),
                  eevdf_status::success);
        reference[i].deadline =
            (uint64_t{item.request} * eevdf_queue::virtual_scale + item.weight - 1) / item.weight;
    }
    std::mt19937 random(42);
    for (size_t step = 0; step < 10000; ++step) {
        const size_t expected = reference_pick(reference);
        ASSERT_LT(expected, reference.size());
        const auto dispatch = state.queue.begin_dispatch();
        ASSERT_EQ(dispatch.status(), eevdf_status::success);
        ASSERT_EQ(dispatch.entity(), &state.entities[expected]);
        const uint32_t service = 1 + random() % 17;
        reference_charge(reference[expected], service);
        ASSERT_EQ(state.queue.complete_dispatch(service), eevdf_status::success);
        for (size_t i = 0; i < reference.size(); ++i) {
            EXPECT_EQ(state.entities[i].virtual_runtime(), reference[i].runtime);
            EXPECT_EQ(state.entities[i].virtual_deadline(), reference[i].deadline);
        }
    }
}

TEST(eevdf, fixed_set_service_tracks_weights) {
    jobs<2> state;
    ASSERT_EQ(state.queue.enqueue(state.entities[0], {1, 4}), eevdf_status::success);
    ASSERT_EQ(state.queue.enqueue(state.entities[1], {3, 4}), eevdf_status::success);
    std::array<int64_t, 2> service{};
    for (size_t i = 0; i < 10000; ++i) {
        const auto dispatch = state.queue.begin_dispatch();
        ASSERT_EQ(dispatch.status(), eevdf_status::success);
        ++service[dispatch.entity() == &state.entities[0] ? 0 : 1];
        ASSERT_EQ(state.queue.complete_dispatch(1), eevdf_status::success);
    }
    const auto difference = 3 * service[0] - service[1];
    EXPECT_GE(difference, -16);
    EXPECT_LE(difference, 16);
}

TEST(eevdf, membership_admission_close_and_dispatch_protocol) {
    jobs<3> state;
    eevdf_queue other;
    EXPECT_EQ(state.queue.begin_dispatch().status(), eevdf_status::empty);
    EXPECT_EQ(state.queue.complete_dispatch(1), eevdf_status::no_dispatch);
    for (auto parameters : {eevdf_parameters{0, 1},
                            {1, 0},
                            {eevdf_queue::max_weight + 1, 1},
                            {1, eevdf_queue::max_service + 1}}) {
        EXPECT_EQ(state.queue.enqueue(state.entities[0], parameters),
                  eevdf_status::invalid_parameters);
        EXPECT_FALSE(state.entities[0].registered());
    }
    ASSERT_EQ(state.queue.enqueue(state.entities[0]), eevdf_status::success);
    EXPECT_EQ(state.queue.enqueue(state.entities[0]), eevdf_status::already_registered);
    EXPECT_EQ(other.enqueue(state.entities[0]), eevdf_status::already_registered);
    EXPECT_EQ(other.cancel(state.entities[0]), eevdf_status::wrong_queue);
    ASSERT_EQ(state.queue.begin_dispatch().status(), eevdf_status::success);
    EXPECT_EQ(state.queue.begin_dispatch().status(), eevdf_status::busy);
    EXPECT_EQ(state.queue.enqueue(state.entities[1]), eevdf_status::busy);
    EXPECT_EQ(state.queue.cancel(state.entities[0]), eevdf_status::busy);
    EXPECT_EQ(state.queue.clear(), eevdf_status::busy);
    EXPECT_EQ(state.queue.complete_dispatch(0), eevdf_status::invalid_service);
    EXPECT_NE(state.queue.current(), nullptr);
    EXPECT_EQ(state.queue.complete_dispatch(1), eevdf_status::success);
    const auto average = state.entities[0].virtual_runtime();
    ASSERT_EQ(state.queue.enqueue(state.entities[1]), eevdf_status::success);
    EXPECT_EQ(state.entities[1].virtual_runtime(), average);
    EXPECT_EQ(state.queue.cancel(state.entities[0]), eevdf_status::success);
    EXPECT_FALSE(state.entities[0].registered());
    state.queue.close();
    EXPECT_TRUE(state.queue.closed());
    EXPECT_EQ(state.queue.enqueue(state.entities[2]), eevdf_status::closed);
    EXPECT_EQ(state.queue.begin_dispatch().entity(), &state.entities[1]);
    EXPECT_EQ(state.queue.complete_dispatch(0, true), eevdf_status::success);
    EXPECT_EQ(state.queue.begin_dispatch().status(), eevdf_status::empty);
}

TEST(eevdf, capacity_and_maximum_weight_request_service) {
    jobs<eevdf_queue::max_entities + 1> state;
    for (size_t i = 0; i < eevdf_queue::max_entities; ++i) {
        ASSERT_EQ(state.queue.enqueue(state.entities[i],
                                      {eevdf_queue::max_weight, eevdf_queue::max_service}),
                  eevdf_status::success);
    }
    EXPECT_EQ(state.queue.enqueue(state.entities.back()), eevdf_status::capacity_limit);
    for (size_t i = 0; i < eevdf_queue::max_entities; ++i) {
        ASSERT_EQ(state.queue.begin_dispatch().entity(), &state.entities[i]);
        ASSERT_EQ(state.queue.complete_dispatch(eevdf_queue::max_service), eevdf_status::success);
        EXPECT_EQ(state.entities[i].virtual_runtime(), uint64_t{eevdf_queue::max_service} *
                                                           eevdf_queue::virtual_scale /
                                                           eevdf_queue::max_weight);
    }
    EXPECT_EQ(state.queue.begin_dispatch().entity(), &state.entities[0]);
}

TEST(eevdf, late_admission_uses_weighted_average_and_removal_updates_eligibility) {
    jobs<3> state;
    auto &first = state.entities[0];
    auto &second = state.entities[1];
    auto &late = state.entities[2];
    ASSERT_EQ(state.queue.enqueue(first, {2, 7}), eevdf_status::success);
    ASSERT_EQ(state.queue.enqueue(second, {5, 3}), eevdf_status::success);
    ASSERT_EQ(state.queue.begin_dispatch().entity(), &second);
    ASSERT_EQ(state.queue.complete_dispatch(6), eevdf_status::success);
    const auto average = (2 * first.virtual_runtime() + 5 * second.virtual_runtime()) / 7;
    ASSERT_EQ(state.queue.enqueue(late, {3, 2}), eevdf_status::success);
    EXPECT_EQ(late.virtual_runtime(), average);
    ASSERT_EQ(state.queue.begin_dispatch().entity(), &late);
    ASSERT_EQ(state.queue.complete_dispatch(1), eevdf_status::success);
    ASSERT_EQ(state.queue.begin_dispatch().entity(), &first);
    ASSERT_EQ(state.queue.complete_dispatch(2), eevdf_status::success);
    ASSERT_EQ(state.queue.cancel(second), eevdf_status::success);
    EXPECT_EQ(state.queue.begin_dispatch().entity(), &first);
}

TEST(eevdf, accounting_failure_is_transactional_and_requires_detachment) {
    for (uint64_t service : {uint64_t{eevdf_queue::max_service} + 1, UINT64_MAX}) {
        jobs<1> state;
        ASSERT_EQ(state.queue.enqueue(state.entities[0]), eevdf_status::success);
        ASSERT_EQ(state.queue.begin_dispatch().status(), eevdf_status::success);
        const auto runtime = state.entities[0].virtual_runtime();
        const auto deadline = state.entities[0].virtual_deadline();
        EXPECT_EQ(state.queue.complete_dispatch(service), eevdf_status::accounting_limit);
        EXPECT_TRUE(state.queue.faulted());
        EXPECT_TRUE(state.queue.closed());
        EXPECT_EQ(state.entities[0].virtual_runtime(), runtime);
        EXPECT_EQ(state.entities[0].virtual_deadline(), deadline);
        EXPECT_EQ(state.queue.begin_dispatch().status(), eevdf_status::faulted);
        EXPECT_EQ(state.queue.clear(), eevdf_status::success);
        EXPECT_FALSE(state.entities[0].registered());
        EXPECT_EQ(state.queue.enqueue(state.entities[0]), eevdf_status::faulted);
    }
}

TEST(eevdf, common_origin_rebase_preserves_selection_and_relative_deadlines) {
    jobs<2> state;
    for (size_t i = 0; i < 2; ++i) {
        ASSERT_EQ(state.queue.enqueue(state.entities[i],
                                      {1, eevdf_queue::max_service / static_cast<uint32_t>(i + 1)}),
                  eevdf_status::success);
    }
    bool rebased = false;
    for (size_t step = 0; step < 8200 && !rebased; ++step) {
        const auto first = state.entities[0].virtual_runtime();
        const auto second = state.entities[1].virtual_runtime();
        const auto first_deadline = state.entities[0].virtual_deadline();
        const auto second_deadline = state.entities[1].virtual_deadline();
        const auto mean = (first + second) / 2;
        const size_t expected =
            first <= mean && (second > mean || first_deadline <= second_deadline) ? 0 : 1;
        ASSERT_EQ(state.queue.begin_dispatch().entity(), &state.entities[expected]);
        if (state.entities[0].virtual_runtime() < first) {
            const auto offset = first - state.entities[0].virtual_runtime();
            EXPECT_EQ(second - state.entities[1].virtual_runtime(), offset);
            EXPECT_EQ(first_deadline - state.entities[0].virtual_deadline(), offset);
            EXPECT_EQ(second_deadline - state.entities[1].virtual_deadline(), offset);
            EXPECT_GE(offset, eevdf_queue::rebase_threshold);
            rebased = true;
        }
        ASSERT_EQ(state.queue.complete_dispatch(eevdf_queue::max_service), eevdf_status::success);
    }
    EXPECT_TRUE(rebased);
}
