#include "LFUPolicy.h"
#include <algorithm>

namespace bufman {

void LFUPolicy::init(std::size_t /*pool_size*/) {
    buckets_.clear();
    slots_.clear();
    min_freq_ = 0;
}

void LFUPolicy::on_load(std::size_t frame) {
    auto& bucket = buckets_[1];

    bucket.push_front(frame);
    slots_[frame] = Slot{1, bucket.begin()};

    min_freq_ = 1;
}

void LFUPolicy::on_access(std::size_t frame) {
    auto slot_it = slots_.find(frame);
    if (slot_it == slots_.end()) {
        return;
    }

    const std::size_t old_count = slot_it->second.count;
    auto bucket_it = buckets_.find(old_count);

    bucket_it->second.erase(slot_it->second.pos);

    if (bucket_it->second.empty()) {
        buckets_.erase(bucket_it);

        if (min_freq_ == old_count) {
            min_freq_ = old_count + 1;
        }
    }

    const std::size_t new_count = old_count + 1;
    auto& new_bucket = buckets_[new_count];

    new_bucket.push_front(frame);
    slot_it->second = Slot{new_count, new_bucket.begin()};
}

void LFUPolicy::on_remove(std::size_t frame) {
    auto slot_it = slots_.find(frame);
    if (slot_it == slots_.end()) {
        return;
    }

    const std::size_t count = slot_it->second.count;
    auto bucket_it = buckets_.find(count);

    bucket_it->second.erase(slot_it->second.pos);

    const bool removed_min_bucket =
        count == min_freq_ && bucket_it->second.empty();

    if (bucket_it->second.empty()) {
        buckets_.erase(bucket_it);
    }

    slots_.erase(slot_it);

    if (slots_.empty()) {
        min_freq_ = 0;
        return;
    }

    if (removed_min_bucket) {
        min_freq_ = 0;

        for (const auto& [frequency, frames] : buckets_) {
            if (!frames.empty() &&
                (min_freq_ == 0 || frequency < min_freq_)) {
                min_freq_ = frequency;
            }
        }
    }
}

std::optional<std::size_t> LFUPolicy::pick_victim(
    const std::vector<std::size_t>& candidates) const {

    std::optional<std::size_t> victim;
    std::size_t best_frequency = 0;

    for (const auto& [frequency, frames] : buckets_) {
        if (victim.has_value() && frequency > best_frequency) {
            continue;
        }

        for (auto it = frames.rbegin(); it != frames.rend(); ++it) {
            const bool eligible =
                std::find(candidates.begin(), candidates.end(), *it)
                != candidates.end();

            if (eligible) {
                if (!victim.has_value() || frequency < best_frequency) {
                    victim = *it;
                    best_frequency = frequency;
                }

                break;
            }
        }
    }

    return victim;
}

}