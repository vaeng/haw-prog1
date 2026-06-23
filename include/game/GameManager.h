#pragma once

#include "engine/Component.h"
#include "engine/Vector2.h"
#include "game/AudioSystem.h"
#include "game/GameState.h"
#include "game/GameView.h"
#include "game/InputSystem.h"
#include "game/TurnSystem.h"


namespace game {

struct BoardProperties {
    int numTiles{};       /// number of tiles in one row/column of the board
    int screenTileSize{}; /// size of each tile in actual pixels on the screen (including scale of
                          /// parent), assumes square tiles

    float worldScale{1.0f}; // render scale
    int workersPerPlayer{};
    int verticalBoardOffset{}; // vertical offset in pixels to account for the board texture not
                               // being centered on the
    engine::Vector2
        boardTopLeft{}; /// position of the top left corner of the board in window coordinates
    engine::Vector2 boardBottomRight{}; /// position of the bottom right corner of the board in
                                        /// window coordinates
};

// GameManager is responsible for managing the game state, including the game board, player, and
// other game objects.
// Note: Assumes a square board for simplicity
class GameManager : public engine::Component {
  public:
    GameManager(int workersPerPlayer, int numTiles);
    void start() override;
    void handleEvent(const std::optional<sf::Event> &event, float deltaTime) override;
    [[nodiscard]] std::unique_ptr<engine::Component> clone() const override;

    const BoardProperties &getBoardProperties() const;

  private:
    BoardProperties _boardProperties{};
    GameStateData _gameStateData{};
    TurnSystem _turnSystem{_gameStateData, _boardProperties};
    GameView _gameView{_gameStateData, _boardProperties};
    InputSystem _inputSystem{_boardProperties};
    AudioSystem _audioSystem{};
    engine::MessageBus *_messageBus{nullptr};
    void computeBoardBounds();
};
} // namespace game
