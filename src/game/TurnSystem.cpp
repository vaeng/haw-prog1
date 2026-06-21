#include "game/TurnSystem.h"
#include "engine/MessageBus.h"
#include <stdexcept>

namespace game {

void TurnSystem::setup(engine::MessageBus *bus) {
    _messageBus = bus;
    _tileClickedConnection = _messageBus->subscribe<TileClickedMessage>(
        [this](const TileClickedMessage &message) { onTileClicked(message); });
    _gameRestartedConnection =
        _messageBus->subscribe<RestartGameMessage>([this](const RestartGameMessage &message) {
            // only reset game if it is currently in a win state, otherwise ignore restart message
            if (_gameStateData.turn == Turn::Player1Win ||
                _gameStateData.turn == Turn::Player2Win) {
                resetGame();
            }
        });
    _messageBus->publish<StateChangedMessage>({}); // trigger initial label and highlight setup
}

void TurnSystem::onTileClicked(TileClickedMessage message) {
    switch (_gameStateData.turn) {
    case Turn::Player1Placement:
        placeWorker(1, message.x, message.y);
        break;
    case Turn::Player2Placement:
        placeWorker(2, message.x, message.y);
        break;
    case Turn::Player1Select:
        selectWorker(1, message.x, message.y);
        break;
    case Turn::Player2Select:
        selectWorker(2, message.x, message.y);
        break;
    case Turn::Player1Movement:
    case Turn::Player2Movement:
        tryMovePlayer(message.x, message.y);
        break;
    case Turn::Player1Build:
    case Turn::Player2Build:
        trySetBuilding(message.x, message.y);
        break;
    default:
        break; // do nothing if it's a win state
    }
}

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
    // Check if the selected worker is on a level 3 tile
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
    updatePossibleActions();
    _messageBus->publish<StateChangedMessage>({}); // trigger view update after state change
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
        if (getSelectedWorker().isPlaced) {
            _messageBus->publish<WorkerMovedMessage>({getSelectedWorker().id,
                                                      getSelectedWorker().position.first,
                                                      getSelectedWorker().position.second, x, y});
        }
        getSelectedWorker().position = {x, y};
        getSelectedWorker().isPlaced = true;
        if (isGameWon()) {
            _gameStateData.turn =
                (getSelectedWorker().playerNumber == 1) ? Turn::Player1Win : Turn::Player2Win;
            // reset all preview
            for (auto &[_, data] : _gameStateData.tileData) {
                data.highlight = HighlightType::None;
            }
            _messageBus->publish<StateChangedMessage>({}); // trigger view update after state change
        } else {
            progressState();
        }
    }
}

void TurnSystem::placeWorker(int playerNumber, int x, int y) {
    // check if tile is already occupied by another worker
    for (const auto &playerData : _gameStateData.workers) {
        if (playerData.isPlaced && x == playerData.position.first &&
            y == playerData.position.second) {
            return; // tile is occupied, do not place worker
        }
    }
    // grab first worker that matches the player number and is not yet placed on the board
    for (auto &playerData : _gameStateData.workers) {
        if (playerData.playerNumber == playerNumber && !playerData.isPlaced) {
            playerData.position = {x, y};
            playerData.isPlaced = true;
            _messageBus->publish<WorkerPlacedMessage>(
                {playerData.id}); // trigger worker placement animation in view
            _messageBus->publish<StateChangedMessage>(
                {}); // trigger view update after placing worker
            break;
        }
    }
    // assuming players place all their workers at once, before the other one starts:
    auto allWorkersPlaced = true;
    for (const auto &playerData : _gameStateData.workers) {
        if (!playerData.isPlaced && playerData.playerNumber == playerNumber) {
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
    _gameStateData.tileData[{x, y}].buildingLevel =
        static_cast<BuildingLevel>(static_cast<int>(currentLevel) + 1);
    _messageBus->publish<BuildingPlacedMessage>(
        {x, y, _gameStateData.tileData[{x, y}].buildingLevel});
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

void TurnSystem::updatePossibleActions() {
    // Clear all highlights first
    for (auto &[_, data] : _gameStateData.tileData) {
        data.highlight = HighlightType::None;
    }
    switch (_gameStateData.turn) {
    case Turn::Player1Movement:
        for (auto &[tilePos, data] : _gameStateData.tileData) {
            if (canPlayerMove(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanMove;
            } else if (isTileAdjacentToPlayer(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::BlockedMove;
            }
        }
        break;
    case Turn::Player1Build:
        for (auto &[tilePos, data] : _gameStateData.tileData) {
            if (canPlayerBuild(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanBuild;
            } else if (isTileAdjacentToPlayer(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::BlockedBuild;
            }
        }
        break;
    case Turn::Player2Movement:
        for (auto &[tilePos, data] : _gameStateData.tileData) {
            if (canPlayerMove(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanMove;
            } else if (isTileAdjacentToPlayer(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::BlockedMove;
            }
        }
        break;
    case Turn::Player2Build:
        for (auto &[tilePos, data] : _gameStateData.tileData) {
            if (canPlayerBuild(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanBuild;
            } else if (isTileAdjacentToPlayer(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::BlockedBuild;
            }
        }
        break;
    default:
        break; // do nothing in placement and win states
    }
}

void TurnSystem::resetGame() {
    // Reset game state
    _gameStateData.turn = Turn::Player1Placement;
    for (auto &workerData : _gameStateData.workers) {
        workerData.isSelected = false;
        workerData.position = {0, 0};
        workerData.isPlaced = false;
    }
    // default select the first worker of player 1 at the start of the game
    _gameStateData.workers[0].isSelected = true;

    // Reset tile data and visuals
    for (auto &[tilePos, data] : _gameStateData.tileData) {
        data.buildingLevel = BuildingLevel::None;
        data.highlight = HighlightType::None;
    }
    _messageBus->publish<StateChangedMessage>({}); // trigger view update after resetting game state
}
} // namespace game
