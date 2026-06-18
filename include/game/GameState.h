#pragma once

#include "engine/GameObject.h"
#include <map>

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
    engine::GameObject *tileObject{nullptr};
    BuildingLevel buildingLevel{BuildingLevel::None};
    HighlightType highlight{HighlightType::None};
};

struct WorkerData {
    std::pair<int, int> position{};
    engine::GameObject *object{nullptr};
    bool isSelected{false};
    int playerNumber{};
};

struct GameStateData {
    Turn turn{};
    std::vector<WorkerData> workers;
    std::map<std::pair<int, int>, TileData> tileData;
};
} // namespace game