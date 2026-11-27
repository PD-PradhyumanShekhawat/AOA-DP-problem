#include "bin_packing.hpp"
#include "water_distribution.hpp"

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

static int run_water() {
    int node_count, plant_count, demand_count, pipe_count;
    if (!(std::cin >> node_count >> plant_count >> demand_count >> pipe_count)) {
        throw std::runtime_error("Expected: N P D M.");
    }
    if (node_count <= 0 || plant_count < 0 || demand_count < 0 || pipe_count < 0) {
        throw std::runtime_error("Invalid network dimensions.");
    }

    std::vector<Supply> supplies(plant_count);
    std::vector<Demand> demands(demand_count);
    std::vector<Pipe> pipes(pipe_count);

    for (auto& supply : supplies) {
        std::cin >> supply.node >> supply.amount;
    }
    for (auto& demand : demands) {
        std::cin >> demand.node >> demand.amount;
    }
    for (auto& pipe : pipes) {
        std::cin >> pipe.from >> pipe.to >> pipe.capacity;
    }
    if (!std::cin) {
        throw std::runtime_error("Input ended before all records were read.");
    }

    const WaterAllocation result = solve_water_distribution(
        node_count, supplies, demands, pipes
    );

    std::cout << "Maximum water delivered: " << result.total_delivered << '\n';
    for (std::size_t i = 0; i < result.delivered_to_zones.size(); ++i) {
        std::cout << "Demand zone " << i << " delivered: "
                  << result.delivered_to_zones[i] << '\n';
    }
    return 0;
}

static int run_binpack() {
    std::size_t item_count;
    std::int64_t capacity;
    if (!(std::cin >> item_count >> capacity)) {
        throw std::runtime_error("Expected: item_count bin_capacity.");
    }

    std::vector<std::int64_t> sizes(item_count);
    for (auto& size : sizes) {
        std::cin >> size;
    }
    if (!std::cin) {
        throw std::runtime_error("Input ended before all item sizes were read.");
    }

    const BinPackingResult result = first_fit_decreasing(sizes, capacity);
    std::cout << "Bins used by First Fit Decreasing: " << result.bins.size() << '\n';

    for (std::size_t b = 0; b < result.bins.size(); ++b) {
        std::cout << "Bin " << b << " (remaining capacity "
                  << result.remaining_capacity[b] << "):";
        for (std::size_t item : result.bins[b]) {
            std::cout << ' ' << item << '(' << sizes[item] << ')';
        }
        std::cout << '\n';
    }
    return 0;
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: cot5405 <water|binpack>\n";
            return 2;
        }

        const std::string mode = argv[1];
        if (mode == "water") {
            return run_water();
        }
        if (mode == "binpack") {
            return run_binpack();
        }

        std::cerr << "Unknown mode: " << mode << '\n';
        return 2;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
