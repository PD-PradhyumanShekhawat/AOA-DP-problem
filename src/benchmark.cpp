#include "bin_packing.hpp"
#include "dinic.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <tuple>
#include <vector>

using Clock = std::chrono::steady_clock;

static double median_ms(std::vector<double> samples) {
    std::sort(samples.begin(), samples.end());
    return samples[samples.size() / 2];
}

static void benchmark_flow() {
    std::ofstream out("flow_benchmark.csv");
    out << "vertices,edges,median_time_ms,V_squared_times_E\n";

    std::mt19937_64 rng(5405);
    for (int n : {1000, 2000, 4000, 8000, 12000}) {
        const int source = 0;
        const int sink = n - 1;
        std::vector<std::pair<int, int>> endpoints;

        // A guaranteed source-to-sink chain, plus random forward edges.
        for (int i = 0; i + 1 < n; ++i) {
            endpoints.emplace_back(i, i + 1);
        }
        std::uniform_int_distribution<int> vertex_dist(0, n - 1);
        for (int i = 0; i < 4 * n; ++i) {
            int u = vertex_dist(rng);
            int v = vertex_dist(rng);
            if (u > v) std::swap(u, v);
            if (u != v) endpoints.emplace_back(u, v);
        }

        std::uniform_int_distribution<int> capacity_dist(1, 100);
        std::vector<std::tuple<int, int, std::int64_t>> graph_edges;
        graph_edges.reserve(endpoints.size());
        for (const auto& [u, v] : endpoints) {
            graph_edges.emplace_back(u, v, capacity_dist(rng));
        }

        std::vector<double> samples;
        for (int repetition = 0; repetition < 7; ++repetition) {
            Dinic network(n);
            for (const auto& [u, v, capacity] : graph_edges) {
                network.add_edge(u, v, capacity);
            }
            const auto start = Clock::now();
            (void)network.max_flow(source, sink);
            const auto stop = Clock::now();
            samples.push_back(
                std::chrono::duration<double, std::milli>(stop - start).count()
            );
        }

        const std::uint64_t vertices = static_cast<std::uint64_t>(n);
        const std::uint64_t edges = static_cast<std::uint64_t>(endpoints.size());
        out << n << ',' << endpoints.size() << ',' << median_ms(samples)
            << ',' << vertices * vertices * edges << '\n';
    }
}

static void benchmark_binpack() {
    std::ofstream out("binpack_benchmark.csv");
    out << "items,median_time_ms,n_log2_n\n";

    std::mt19937_64 rng(5406);
    constexpr std::int64_t capacity = 1000;
    std::uniform_int_distribution<std::int64_t> size_dist(1, capacity);

    for (int n : {1000, 2000, 4000, 8000, 16000, 32000}) {
        std::vector<std::int64_t> items(n);
        for (auto& item : items) item = size_dist(rng);

        std::vector<double> samples;
        for (int repetition = 0; repetition < 7; ++repetition) {
            const auto start = Clock::now();
            (void)first_fit_decreasing(items, capacity);
            const auto stop = Clock::now();
            samples.push_back(
                std::chrono::duration<double, std::milli>(stop - start).count()
            );
        }

        const double n_log_n = static_cast<double>(n) *
            (std::log2(static_cast<double>(n)));
        out << n << ',' << median_ms(samples) << ',' << n_log_n << '\n';
    }
}

int main() {
    benchmark_flow();
    benchmark_binpack();
    std::cout << "Wrote flow_benchmark.csv and binpack_benchmark.csv\n";
    return 0;
}
