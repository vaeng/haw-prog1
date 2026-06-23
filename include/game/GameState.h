#pragma once

#include <cstdint>
#include <map>
#include <vector>

namespace game {
enum class Turn : uint8_t {
    Player1Placement,
    Player2Placement,
    Player1Select,
    Player2Select,
    Player1Movement,
    Player2Movement,
    Player1Build,
    Player2Build,
    Player1Win,
    Player2Win
};

enum class BuildingLevel : uint8_t {
    None,
    Level1,
    Level2,
    Level3,
    Dome
}; // move 11 down, then right

enum class HighlightType : uint8_t { None, CanMove, CanBuild, BlockedMove, BlockedBuild };

struct TileData {
    BuildingLevel buildingLevel{BuildingLevel::None};
    HighlightType highlight{HighlightType::None};
};

struct WorkerData {
    int id{};
    std::pair<int, int> position{};
    bool isSelected{false};
    bool isPlaced{false};
    int playerNumber{};
};

struct GameStateData {
    Turn turn{Turn::Player1Placement}; /// current turn
    std::vector<WorkerData> workers{};
    std::map<std::pair<int, int>, TileData> tileData{};
};
} // namespace game