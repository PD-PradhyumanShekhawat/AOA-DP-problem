#include "bin_packing.hpp"
#include "dinic.hpp"
#include "water_distribution.hpp"

#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <queue>
#include <random>
#include <set>
#include <stdexcept>
#include <tuple>
#include <vector>

#define CHECK(condition)                                                     \
    do {                                                                   \
        if (!(condition)) {                                                 \
            std::cerr << "TEST FAILURE: " << #condition                     \
                      << " at " << __FILE__ << ":" << __LINE__ << '\n';     \
            std::exit(EXIT_FAILURE);                                        \
        }                                                                  \
    } while (false)

static void test_dinic_classic_network() {
    Dinic network(6);
    network.add_edge(0, 1, 16);
    network.add_edge(0, 2, 13);
    network.add_edge(1, 2, 10);
    network.add_edge(2, 1, 4);
    network.add_edge(1, 3, 12);
    network.add_edge(3, 2, 9);
    network.add_edge(2, 4, 14);
    network.add_edge(4, 3, 7);
    network.add_edge(3, 5, 20);
    network.add_edge(4, 5, 4);
    CHECK(network.max_flow(0, 5) == 23);
}

static void test_water_distribution() {
    const std::vector<Supply> supplies{{0, 10}};
    const std::vector<Demand> demands{{3, 8}};
    const std::vector<Pipe> pipes{
        {0, 1, 6}, {0, 2, 6}, {1, 3, 5}, {2, 3, 7}
    };

    const auto result = solve_water_distribution(4, supplies, demands, pipes);
    CHECK(result.total_delivered == 8);
    CHECK(result.delivered_to_zones.size() == 1);
    CHECK(result.delivered_to_zones[0] == 8);
}

static void test_water_pipe_limited() {
    const std::vector<Supply> supplies{{0, 20}};
    const std::vector<Demand> demands{{2, 20}};
    const std::vector<Pipe> pipes{{0, 1, 7}, {1, 2, 5}};

    const auto result = solve_water_distribution(3, supplies, demands, pipes);
    CHECK(result.total_delivered == 5);
    CHECK(result.delivered_to_zones[0] == 5);
}

static void test_water_zero_capacity() {
    const std::vector<Supply> supplies{{0, 10}};
    const std::vector<Demand> demands{{1, 10}};
    const std::vector<Pipe> pipes{{0, 1, 0}};

    const auto result = solve_water_distribution(2, supplies, demands, pipes);
    CHECK(result.total_delivered == 0);
    CHECK(result.delivered_to_zones.size() == 1);
    CHECK(result.delivered_to_zones[0] == 0);
}

static void test_water_unreachable_demand() {
    const std::vector<Supply> supplies{{0, 10}};
    const std::vector<Demand> demands{{2, 10}};
    const std::vector<Pipe> pipes{{0, 1, 5}};

    const auto result = solve_water_distribution(3, supplies, demands, pipes);
    CHECK(result.total_delivered == 0);
    CHECK(result.delivered_to_zones[0] == 0);
}

static void test_water_multiple_supplies_and_demands() {
    const std::vector<Supply> supplies{{0, 5}, {1, 7}};
    const std::vector<Demand> demands{{3, 4}, {4, 8}};
    const std::vector<Pipe> pipes{
        {0, 2, 5},
        {1, 2, 7},
        {2, 3, 4},
        {2, 4, 8}
    };

    const auto result = solve_water_distribution(5, supplies, demands, pipes);

    CHECK(result.total_delivered == 12);
    CHECK(result.delivered_to_zones.size() == 2);
    CHECK(result.delivered_to_zones[0] == 4);
    CHECK(result.delivered_to_zones[1] == 8);
}

static void test_water_parallel_pipes() {
    const std::vector<Supply> supplies{{0, 10}};
    const std::vector<Demand> demands{{1, 10}};
    const std::vector<Pipe> pipes{
        {0, 1, 3},
        {0, 1, 4}
    };

    const auto result = solve_water_distribution(2, supplies, demands, pipes);

    CHECK(result.total_delivered == 7);
    CHECK(result.delivered_to_zones[0] == 7);
}

static void test_water_rejects_invalid_node() {
    bool threw = false;

    try {
        const std::vector<Supply> supplies{{2, 10}};
        const std::vector<Demand> demands{{1, 10}};
        const std::vector<Pipe> pipes;

        (void)solve_water_distribution(2, supplies, demands, pipes);
    } catch (const std::out_of_range&) {
        threw = true;
    }

    CHECK(threw);
}

static void test_water_rejects_negative_capacity() {
    bool threw = false;

    try {
        const std::vector<Supply> supplies{{0, 10}};
        const std::vector<Demand> demands{{1, 10}};
        const std::vector<Pipe> pipes{{0, 1, -1}};

        (void)solve_water_distribution(2, supplies, demands, pipes);
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    CHECK(threw);
}

static void test_ffd_feasibility() {
    const std::vector<std::int64_t> sizes{7, 5, 5, 3, 3, 2};
    const auto result = first_fit_decreasing(sizes, 10);

    std::vector<int> seen(sizes.size(), 0);
    for (std::size_t b = 0; b < result.bins.size(); ++b) {
        std::int64_t used = 0;
        for (std::size_t item : result.bins[b]) {
            ++seen[item];
            used += sizes[item];
        }
        CHECK(used <= 10);
        CHECK(result.remaining_capacity[b] == 10 - used);
    }
    for (int count : seen) {
        CHECK(count == 1);
    }
    CHECK(result.bins.size() == 3);
}

static void test_ffd_empty_and_single_item() {
    const auto empty = first_fit_decreasing({}, 10);
    CHECK(empty.bins.empty());

    const auto single = first_fit_decreasing({10}, 10);
    CHECK(single.bins.size() == 1);
    CHECK(single.remaining_capacity[0] == 0);
}

static void test_ffd_rejects_oversized_item() {
    bool threw = false;
    try {
        (void)first_fit_decreasing({11}, 10);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    CHECK(threw);
}


static std::int64_t edmonds_karp_reference(
    int n, const std::vector<std::tuple<int, int, std::int64_t>>& edges,
    int source, int sink
) {
    std::vector<std::vector<std::int64_t>> residual(
        n, std::vector<std::int64_t>(n, 0)
    );
    for (const auto& [u, v, capacity] : edges) residual[u][v] += capacity;

    std::int64_t total = 0;
    while (true) {
        std::vector<int> parent(n, -1);
        std::queue<int> q;
        parent[source] = source;
        q.push(source);
        while (!q.empty() && parent[sink] == -1) {
            int u = q.front(); q.pop();
            for (int v = 0; v < n; ++v) {
                if (parent[v] == -1 && residual[u][v] > 0) {
                    parent[v] = u;
                    q.push(v);
                }
            }
        }
        if (parent[sink] == -1) break;
        std::int64_t pushed = INT64_MAX;
        for (int v = sink; v != source; v = parent[v])
            pushed = std::min(pushed, residual[parent[v]][v]);
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            residual[u][v] -= pushed;
            residual[v][u] += pushed;
        }
        total += pushed;
    }
    return total;
}

static void test_randomized_flow_against_reference() {
    std::mt19937 rng(5405);
    for (int trial = 0; trial < 300; ++trial) {
        const int n = 2 + static_cast<int>(rng() % 7);
        std::vector<std::tuple<int, int, std::int64_t>> edges;
        Dinic network(n);
        for (int u = 0; u < n; ++u) {
            for (int v = 0; v < n; ++v) {
                if (u != v && rng() % 5 == 0) {
                    const std::int64_t cap = 1 + rng() % 20;
                    edges.emplace_back(u, v, cap);
                    network.add_edge(u, v, cap);
                }
            }
        }
        const auto expected = edmonds_karp_reference(n, edges, 0, n - 1);
        const auto actual = network.max_flow(0, n - 1);
        CHECK(actual == expected);
    }
}

static BinPackingResult naive_ffd_reference(
    const std::vector<std::int64_t>& sizes, std::int64_t capacity
) {
    std::vector<std::pair<std::int64_t, std::size_t>> order;
    for (std::size_t i = 0; i < sizes.size(); ++i)
        order.emplace_back(sizes[i], i);
    std::sort(order.begin(), order.end(), [](const auto& a, const auto& b) {
        if (a.first != b.first) return a.first > b.first;
        return a.second < b.second;
    });

    BinPackingResult result;
    for (const auto& [size, original] : order) {
        std::size_t b = 0;
        while (b < result.remaining_capacity.size() &&
               result.remaining_capacity[b] < size) ++b;
        if (b == result.remaining_capacity.size()) {
            result.bins.emplace_back();
            result.remaining_capacity.push_back(capacity);
        }
        result.bins[b].push_back(original);
        result.remaining_capacity[b] -= size;
    }
    return result;
}

static void test_randomized_ffd_against_naive_reference() {
    std::mt19937 rng(5406);
    for (int trial = 0; trial < 500; ++trial) {
        const int n = static_cast<int>(rng() % 80);
        const std::int64_t capacity = 1 + rng() % 100;
        std::vector<std::int64_t> sizes(n);
        for (auto& size : sizes) size = 1 + rng() % capacity;

        const auto actual = first_fit_decreasing(sizes, capacity);
        const auto expected = naive_ffd_reference(sizes, capacity);
        CHECK(actual.bins.size() == expected.bins.size());
        CHECK(actual.remaining_capacity == expected.remaining_capacity);
        CHECK(actual.bins == expected.bins);
    }
}

int main() {
    test_dinic_classic_network();
    test_water_distribution();
    test_water_pipe_limited();
    test_ffd_feasibility();
    test_ffd_empty_and_single_item();
    test_ffd_rejects_oversized_item();
    test_randomized_flow_against_reference();
    test_randomized_ffd_against_naive_reference();
    test_water_zero_capacity();
    test_water_unreachable_demand();
    test_water_multiple_supplies_and_demands();
    test_water_parallel_pipes();
    test_water_rejects_invalid_node();
    test_water_rejects_negative_capacity();

    std::cout << "All project tests passed.\n";
    return 0;
}
