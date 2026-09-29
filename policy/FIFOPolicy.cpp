#include "FIFOPolicy.h"

#include <algorithm>

namespace bufman {

    void FIFOPolicy::init(std::size_t pool_size) {
        (void)pool_size;
        queue_.clear();
        positions_.clear();
    }

    void FIFOPolicy::on_access(std::size_t frame) {
        (void)frame;
    }

    void FIFOPolicy::on_load(std::size_t frame) {
        // avoid adding frame more than once
        if (positions_.find(frame) != positions_.end()) {
            return;
        }
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