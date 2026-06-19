#pragma once

#include "engine/Component.h"
#include "engine/GameObject.h"

namespace game {

class GameStateData;
class BoardProperties;

class GameView {
  public:
    GameView(GameStateData &gameStateData, const BoardProperties &boardProperties)
        : _gameStateData(gameStateData), _boardProperties(boardProperties) {}
    void setupLabels(engine::GameObject *owner);
    void updateLabels();
    void updateBoard();
    void highlightPossibleActions();

  private:
    GameStateData &_gameStateData;
    const BoardProperties &_boardProperties;
    int _textureTileSize{32}; /// size of the board tiles in pixels, hardcoded for now
    engine::GameObject *_activePlayerLabel{nullptr};
    engine::GameObject *_gameStateLabel{nullptr};
    engine::GameObject *_restartLabel{nullptr};
};

} // namespace game