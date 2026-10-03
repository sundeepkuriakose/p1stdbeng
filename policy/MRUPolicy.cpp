#include "MRUPolicy.h"

#include <algorithm>

namespace bufman {
    void MRUPolicy::init(std::size_t pool_size) {
        order_.clear(); // access history
        positions_.assign(pool_size, order_.end());
    }

    // when a frame is accessed or loaded make it the MRU
    void MRUPolicy::touch(std::size_t frame) {
    if (frame >= positions_.size()) {
        return;
    }

    if (positions_[frame] != order_.end()) {
        order_.erase(positions_[frame]);
    }

    order_.push_front(frame);
    positions_[frame] = order_.begin();
}

    void MRUPolicy::on_access(std::size_t frame) {
        touch(frame);
    }

    void MRUPolicy::on_load(std::size_t frame) {
        touch(frame);
    }

    void MRUPolicy::on_remove(std::size_t frame) {
    if (frame >= positions_.size()) {
        return;
    }

    if (positions_[frame] != order_.end()) {
        order_.erase(positions_[frame]);
        positions_[frame] = order_.end();
    }
}

    // optional means it might return a frame number or nothing
    // &: refer to existing vector not make a copy
    // second const: promises not to modify MRUPolicy object's (order_ and positions_) data
    std::optional<std::size_t> MRUPolicy::pick_victim(
        const std::vector<std::size_t>& candidates) const {
        // for elements in order_ going from the start (MRU aspect)
        for (auto it = order_.begin(); it != order_.end(); ++it) {
            // the first candidate found in the access history (order_) is our victim
            if (std::find(candidates.begin(), candidates.end(), *it) != candidates.end()) {
                return *it;
            }
        }
        return std::nullopt;
    }

}