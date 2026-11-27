#pragma once

#include "dinic.hpp"

#include <cstdint>
#include <stdexcept>
#include <vector>

struct Supply {
    int node;
    std::int64_t amount;
};

struct Demand {
    int node;
    std::int64_t amount;
};

struct Pipe {
    int from;
    int to;
    std::int64_t capacity;
};

struct WaterAllocation {
    std::int64_t total_delivered = 0;
    std::vector<std::int64_t> delivered_to_zones;
};

inline WaterAllocation solve_water_distribution(
    int physical_node_count,
    const std::vector<Supply>& supplies,
    const std::vector<Demand>& demands,
    const std::vector<Pipe>& pipes
) {
    if (physical_node_count <= 0) {
        throw std::invalid_argument("The physical network must have at least one node.");
    }
        // Validate all physical nodes and capacities before constructing the flow network.
    for (const Supply& supply : supplies) {
        if (supply.node < 0 || supply.node >= physical_node_count) {
            throw std::out_of_range("Supply node is outside the physical network.");
        }
        if (supply.amount < 0) {
            throw std::invalid_argument("Supply amount cannot be negative.");
        }
    }

    for (const Demand& demand : demands) {
        if (demand.node < 0 || demand.node >= physical_node_count) {
            throw std::out_of_range("Demand node is outside the physical network.");
        }
        if (demand.amount < 0) {
            throw std::invalid_argument("Demand amount cannot be negative.");
        }
    }

    for (const Pipe& pipe : pipes) {
        if (pipe.from < 0 || pipe.from >= physical_node_count ||
            pipe.to < 0 || pipe.to >= physical_node_count) {
            throw std::out_of_range("Pipe endpoint is outside the physical network.");
        }
        if (pipe.capacity < 0) {
            throw std::invalid_argument("Pipe capacity cannot be negative.");
        }
    }

    const int super_source = physical_node_count;
    const int super_sink = physical_node_count + 1;
    Dinic network(physical_node_count + 2);

    for (const Supply& supply : supplies) {
        network.add_edge(super_source, supply.node, supply.amount);
    }

    std::vector<Dinic::EdgeHandle> demand_handles;
    demand_handles.reserve(demands.size());

    for (const Demand& demand : demands) {
        demand_handles.push_back(
            network.add_edge(demand.node, super_sink, demand.amount)
        );
    }

    for (const Pipe& pipe : pipes) {
        network.add_edge(pipe.from, pipe.to, pipe.capacity);
    }

    WaterAllocation result;
    result.total_delivered = network.max_flow(super_source, super_sink);
    result.delivered_to_zones.reserve(demand_handles.size());

    for (const auto& handle : demand_handles) {
        result.delivered_to_zones.push_back(network.flow_on(handle));
    }

    return result;
}
