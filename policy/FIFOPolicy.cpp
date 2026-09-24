#include "FIFOPolicy.h"

#include <algorithm>

namespace bufman {

    void FIFOPolicy::init(std::size_t pool_size) {
        queue_.clear();
        positions_.clear();
    }

    void FIFOPolicy::on_access(std::size_t frame) {
    }

    void FIFOPolicy::on_load(std::size_t frame) {
        queue_.push_back(frame);
        positions_[frame] = std::prev(queue_.end());
    }

    void FIFOPolicy::on_remove(std::size_t frame) {
        auto it = positions_.find(frame);

        if (it != positions_.end()) {
            queue_.erase(it->second);
            positions_.erase(it);
        }
    }

    std::optional<std::size_t> FIFOPolicy::pick_victim(
            const std::vector<std::size_t>& candidates) const {
        for (auto it = queue_.begin(); it != queue_.end(); ++it) {
            if (std::find(candidates.begin(), candidates.end(), *it) != candidates.end()) {
                return *it;
            }
        }

        return std::nullopt;
    }

}