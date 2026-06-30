#pragma once

#include <cstdint>
#include <random>
#include <unordered_map>

namespace engine {
struct UUID {
    uint64_t high{};
    uint64_t low{};

    friend auto operator==(const UUID &lhs, const UUID &rhs) -> bool {
        return lhs.high == rhs.high && lhs.low == rhs.low;
    }
    friend auto operator!=(const UUID &lhs, const UUID &rhs) -> bool { return !(lhs == rhs); }
    friend auto operator<(const UUID &lhs, const UUID &rhs) -> bool {
        if (lhs.high < rhs.high) {
            return true;
        } else if (lhs.high > rhs.high) {
            return false;
        } else {
            return lhs.low < rhs.low;
        }
    }

    static UUID generate() {
        static std::mt19937_64 rng(std::random_device{}());
        return {rng(), rng()};
    }
};
} // namespace engine

// std::hash specialization for unordered_map
template <> struct std::hash<engine::UUID> {
    size_t operator()(const engine::UUID &uuid) const noexcept {
        return std::hash<uint64_t>{}(uuid.high) ^ (std::hash<uint64_t>{}(uuid.low) << 1);
    }
};
