#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

class MaxSegmentTree {
public:
    explicit MaxSegmentTree(int capacity)
        : size_(1), tree_() {
        while (size_ < capacity) {
            size_ *= 2;
        }
        tree_.assign(2 * size_, -1);
    }

    void update(int index, std::int64_t value) {
        int position = index + size_;
        tree_[position] = value;
        position /= 2;
        while (position > 0) {
            tree_[position] = std::max(tree_[2 * position], tree_[2 * position + 1]);
            position /= 2;
        }
    }

    int first_at_least(std::int64_t required) const {
        if (tree_[1] < required) {
            return -1;
        }

        int position = 1;
        while (position < size_) {
            if (tree_[2 * position] >= required) {
                position *= 2;
            } else {
                position = 2 * position + 1;
            }
        }
        return position - size_;
    }

private:
    int size_;
    std::vector<std::int64_t> tree_;
};

struct BinPackingResult {
    std::vector<std::vector<std::size_t>> bins;
    std::vector<std::int64_t> remaining_capacity;
};

inline BinPackingResult first_fit_decreasing(
    const std::vector<std::int64_t>& item_sizes,
    std::int64_t bin_capacity
) {
    if (bin_capacity <= 0) {
        throw std::invalid_argument("Bin capacity must be positive.");
    }

    std::vector<std::pair<std::int64_t, std::size_t>> ordered;
    ordered.reserve(item_sizes.size());

    for (std::size_t i = 0; i < item_sizes.size(); ++i) {
        if (item_sizes[i] <= 0 || item_sizes[i] > bin_capacity) {
            throw std::invalid_argument("Every item must have size in [1, bin_capacity].");
        }
        ordered.emplace_back(item_sizes[i], i);
    }

    std::sort(ordered.begin(), ordered.end(),
        [](const auto& left, const auto& right) {
            if (left.first != right.first) {
                return left.first > right.first;
            }
            return left.second < right.second;
        });

    BinPackingResult result;
    result.bins.reserve(item_sizes.size());
    result.remaining_capacity.reserve(item_sizes.size());

    MaxSegmentTree tree(static_cast<int>(item_sizes.size()));

    for (const auto& [size, original_index] : ordered) {
        int bin_index = tree.first_at_least(size);

        if (bin_index == -1) {
            bin_index = static_cast<int>(result.bins.size());
            result.bins.emplace_back();
            result.bins.back().push_back(original_index);
            result.remaining_capacity.push_back(bin_capacity - size);
        } else {
            result.remaining_capacity[bin_index] -= size;
            result.bins[bin_index].push_back(original_index);
        }

        // Activate a new bin or update the remaining capacity of an existing bin.
        tree.update(bin_index, result.remaining_capacity[bin_index]);
    }

    return result;
}
