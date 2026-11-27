#pragma once

#include <algorithm>
#include <cstdint>
#include <limits>
#include <queue>
#include <stdexcept>
#include <vector>

class Dinic {
public:
    using Capacity = std::int64_t;

    struct EdgeHandle {
        int from;
        int index;
    };

    explicit Dinic(int vertex_count)
        : graph_(vertex_count), level_(vertex_count), next_edge_(vertex_count) {
        if (vertex_count <= 0) {
            throw std::invalid_argument("A flow network must contain at least one vertex.");
        }
    }

    EdgeHandle add_edge(int from, int to, Capacity capacity) {
        check_vertex(from);
        check_vertex(to);
        if (from == to) {
            throw std::invalid_argument("Self-loops are not supported by this implementation.");
        }
        if (capacity < 0) {
            throw std::invalid_argument("Edge capacity cannot be negative.");
        }

        const int forward_index = static_cast<int>(graph_[from].size());
        const int reverse_index = static_cast<int>(graph_[to].size());

        graph_[from].push_back(Edge{to, reverse_index, capacity, capacity});
        graph_[to].push_back(Edge{from, forward_index, 0, 0});
        return EdgeHandle{from, forward_index};
    }

    Capacity max_flow(int source, int sink) {
        check_vertex(source);
        check_vertex(sink);
        if (source == sink) {
            throw std::invalid_argument("Source and sink must be different vertices.");
        }

        Capacity total_flow = 0;
        while (build_levels(source, sink)) {
            std::fill(next_edge_.begin(), next_edge_.end(), 0);
            while (true) {
                const Capacity pushed = send_flow(
                    source, sink, std::numeric_limits<Capacity>::max()
                );
                if (pushed == 0) {
                    break;
                }
                if (total_flow > std::numeric_limits<Capacity>::max() - pushed) {
                    throw std::overflow_error("Maximum flow exceeds int64_t capacity.");
                }
                total_flow += pushed;
            }
        }
        return total_flow;
    }

    Capacity flow_on(EdgeHandle handle) const {
        check_vertex(handle.from);
        if (handle.index < 0 ||
            handle.index >= static_cast<int>(graph_[handle.from].size())) {
            throw std::out_of_range("Invalid edge handle.");
        }
        const Edge& edge = graph_[handle.from][handle.index];
        return edge.original_capacity - edge.residual_capacity;
    }

    int vertex_count() const {
        return static_cast<int>(graph_.size());
    }

private:
    struct Edge {
        int to;
        int reverse_index;
        Capacity residual_capacity;
        Capacity original_capacity;
    };

    std::vector<std::vector<Edge>> graph_;
    std::vector<int> level_;
    std::vector<int> next_edge_;

    void check_vertex(int vertex) const {
        if (vertex < 0 || vertex >= static_cast<int>(graph_.size())) {
            throw std::out_of_range("Vertex index is outside the network.");
        }
    }

    bool build_levels(int source, int sink) {
        std::fill(level_.begin(), level_.end(), -1);
        std::queue<int> queue;
        level_[source] = 0;
        queue.push(source);

        while (!queue.empty()) {
            const int vertex = queue.front();
            queue.pop();

            for (const Edge& edge : graph_[vertex]) {
                if (edge.residual_capacity > 0 && level_[edge.to] == -1) {
                    level_[edge.to] = level_[vertex] + 1;
                    queue.push(edge.to);
                }
            }
        }
        return level_[sink] != -1;
    }

    Capacity send_flow(int vertex, int sink, Capacity pushed) {
        if (pushed == 0 || vertex == sink) {
            return pushed;
        }

        for (int& edge_index = next_edge_[vertex];
             edge_index < static_cast<int>(graph_[vertex].size());
             ++edge_index) {
            Edge& edge = graph_[vertex][edge_index];
            if (edge.residual_capacity <= 0 ||
                level_[edge.to] != level_[vertex] + 1) {
                continue;
            }

            const Capacity amount = send_flow(
                edge.to, sink, std::min(pushed, edge.residual_capacity)
            );
            if (amount == 0) {
                continue;
            }

            edge.residual_capacity -= amount;
            graph_[edge.to][edge.reverse_index].residual_capacity += amount;
            return amount;
        }
        return 0;
    }
};
