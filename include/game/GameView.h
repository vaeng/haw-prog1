#pragma once

#include "engine/Component.h"
#include "engine/GameObject.h"
#include "engine/MessageBus.h"

#include <map>

namespace game {

class GameStateData;
class BoardProperties;

class GameView {
  public:
    GameView(GameStateData &gameStateData, const BoardProperties &boardProperties)
        : _gameStateData(gameStateData), _boardProperties(boardProperties) {}
    void setup(engine::GameObject *owner, engine::MessageBus *bus);

  private:
    GameStateData &_gameStateData;
    const BoardProperties &_boardProperties;
    engine::MessageBus *_messageBus{nullptr};
    engine::Connection _stateChangedConnection;

    int _textureTileSize{32}; /// size of the board tiles in pixels, hardcoded for now
    engine::GameObject *_activePlayerLabel{nullptr};
    engine::GameObject *_gameStateLabel{nullptr};
    engine::GameObject *_restartLabel{nullptr};

    std::map<std::pair<int, int>, engine::GameObject *>
        _tileObjects; // map of tile positions to their GameObjects for access when updating
                      // highlights and building levels
    std::map<int, engine::GameObject *>
        _workerObjects; // list of worker GameObjects, indexed by worker id for access when updating
                        // worker positions
    void updateLabels();
    void updateBoard();
    void updatePlayerPositions();
    void highlightPossibleActions();
    void setupLabels(engine::GameObject *owner);
    void createPlayers(engine::GameObject *owner);
    void createBoard(engine::GameObject *owner);
};

} // namespace game