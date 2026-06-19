#include "game/TurnSystem.h"
#include "engine/RenderComponent.h"

namespace game {

bool TurnSystem::isTileAdjacentToPlayer(int x, int y) {
    auto playerPos = getSelectedWorker().position;
    int dx = std::abs(playerPos.first - x);
    int dy = std::abs(playerPos.second - y);
    return (dx <= 1 && dy <= 1) &&
           !(dx == 0 && dy == 0); // adjacent if within 1 tile and not the same tile
}

bool TurnSystem::canPlayerBuild(int x, int y) {
    bool occupiedByAnyWorker = false;
    for (const auto &playerData : _gameStateData.workers) {
        if (x == playerData.position.first && y == playerData.position.second) {
            occupiedByAnyWorker = true;
            break;
        }
    }

    return isTileAdjacentToPlayer(x, y) &&
           _gameStateData.tileData[{x, y}].buildingLevel != BuildingLevel::Dome &&
           !occupiedByAnyWorker;
}

bool TurnSystem::canPlayerMove(int x, int y) {
    bool adjacentToPlayer = isTileAdjacentToPlayer(x, y);
    if (!adjacentToPlayer) {
        return false;
    }
    bool occupiedByAnyWorker = false;
    for (const auto &playerData : _gameStateData.workers) {
        if (x == playerData.position.first && y == playerData.position.second) {
            occupiedByAnyWorker = true;
            break;
        }
    }
    if (occupiedByAnyWorker) {
        return false;
    }
    auto targetLevel = _gameStateData.tileData[{x, y}].buildingLevel;
    bool hasDome = targetLevel == BuildingLevel::Dome;
    if (hasDome) {
        return false;
    }
    BuildingLevel currentLevel =
        _gameStateData
            .tileData[{getSelectedWorker().position.first, getSelectedWorker().position.second}]
            .buildingLevel;

    if (static_cast<int>(targetLevel) - static_cast<int>(currentLevel) > 1) {
        return false; // cannot move up more than one level
    }
    return true;
}

bool TurnSystem::isGameWon() {
    return _gameStateData
               .tileData[{getSelectedWorker().position.first, getSelectedWorker().position.second}]
               .buildingLevel == BuildingLevel::Level3;
}

void TurnSystem::progressState() {
    switch (_gameStateData.turn) {
    case Turn::Player1Placement:
        _gameStateData.turn = Turn::Player2Placement;
        break;
    case Turn::Player2Placement:
        _gameStateData.turn = Turn::Player1Select;
        break;
    case Turn::Player1Select:
        _gameStateData.turn = Turn::Player1Movement;
        break;
    case Turn::Player1Movement:
        _gameStateData.turn = Turn::Player1Build;
        break;
    case Turn::Player1Build:
        _gameStateData.turn = Turn::Player2Select;
        break;
    case Turn::Player2Select:
        _gameStateData.turn = Turn::Player2Movement;
        break;
    case Turn::Player2Movement:
        _gameStateData.turn = Turn::Player2Build;
        break;
    case Turn::Player2Build:
        _gameStateData.turn = Turn::Player1Select; // loop back to movement phase
        break;
    default:
        break; // do nothing in win states
    }
}

void TurnSystem::selectWorker(int playerNumber, int x, int y) {
    for (auto &workerData : _gameStateData.workers) {
        if (workerData.playerNumber == playerNumber && workerData.position.first == x &&
            workerData.position.second == y) {
            getSelectedWorker().isSelected = false;
            workerData.isSelected = true;
            progressState();
            break;
        }
    }
}

void TurnSystem::tryMovePlayer(int x, int y) {
    if (canPlayerMove(x, y)) {
        getSelectedWorker().position = {x, y};
        getSelectedWorker().object->enabled = true; // enable player once it is placed on the board
        moveToTile(getSelectedWorker().object, x, y);

        if (isGameWon()) {
            _gameStateData.turn =
                (getSelectedWorker().playerNumber == 1) ? Turn::Player1Win : Turn::Player2Win;
        } else {
            progressState();
        }
    }
}

void TurnSystem::placeWorker(int playerNumber, int x, int y) {
    // check if tile is already occupied by another worker
    for (const auto &playerData : _gameStateData.workers) {
        if (x == playerData.position.first && y == playerData.position.second) {
            return; // tile is occupied, do not place worker
        }
    }
    // grab first worker that matches the player number and is not yet placed on the board
    for (auto &playerData : _gameStateData.workers) {
        if (playerData.playerNumber == playerNumber && playerData.object->enabled == false) {
            playerData.position = {x, y};
            playerData.object->enabled = true; // enable player once it is placed on the board
            moveToTile(playerData.object, x, y);
            break;
        }
    }
    // assuming players place all their workers at once, before the other one starts:
    auto allWorkersPlaced = true;
    for (const auto &playerData : _gameStateData.workers) {
        if (!playerData.object->enabled && playerData.playerNumber == playerNumber) {
            allWorkersPlaced = false;
            break;
        }
    }
    if (allWorkersPlaced) {
        progressState();
    }
}

void TurnSystem::trySetBuilding(int x, int y) {
    if (canPlayerBuild(x, y)) {
        placeBuilding(x, y);
    }
}

void TurnSystem::placeBuilding(int x, int y) {
    auto currentLevel = _gameStateData.tileData[{x, y}].buildingLevel;
    auto tile = _gameStateData.tileData[{x, y}].tileObject;
    if (tile == nullptr) {
        return;
    }
    auto rc = tile->getComponent<engine::RenderComponent>();
    if (rc != nullptr) {
        auto previousRect = rc->getTextureRect();
        if (currentLevel == BuildingLevel::None) {
            previousRect.top += 11 * _textureTileSize;
        } else {
            previousRect.left += _textureTileSize;
        }
        rc->setTextureRect(previousRect);
    }
    _gameStateData.tileData[{x, y}].buildingLevel =
        static_cast<BuildingLevel>(static_cast<int>(currentLevel) + 1);
    progressState();
}

WorkerData &TurnSystem::getSelectedWorker() {
    for (auto &worker : _gameStateData.workers) {
        if (worker.isSelected) {
            return worker;
        }
    }
    throw std::runtime_error("No worker is currently selected");
}

void TurnSystem::moveToTile(engine::GameObject *player, int x, int y) {
    auto tile = _gameStateData.tileData[{x, y}].tileObject;
    if (tile == nullptr) {
        printf("No tile at (%d, %d)\n", x, y);
        return;
    }
    player->localTransform.position = {tile->localTransform.position.x,
                                       tile->localTransform.position.y};
}

} // namespace game
