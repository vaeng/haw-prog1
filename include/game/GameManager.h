#pragma once

#include <SFML/Audio/Sound.hpp>

#include "engine/Component.h"
#include "engine/Vector2.h"
#include "game/GameState.h"
#include "game/GameView.h"
#include "game/TurnSystem.h"

namespace game {

struct BoardProperties {
    int numTiles{};       /// number of tiles in one row/column of the board
    int screenTileSize{}; /// size of each tile in actual pixels on the screen (including scale of
                          /// parent), assumes square tiles
    int workersPerPlayer{};
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
    int _textureTileSize{}; /// size of the board tiles in pixels, hardcoded in start() for now,
                            /// could be made configurable

    void computeBoardBounds();
    void createPlayers();
    void createBoard();

    void onHoverOvertile(int x, int y);

    void onClickTile(int x, int y);

    void restartGame();

    std::tuple<bool, int, int> getTileUnderMouse(int mouseX, int mouseY);
};
} // namespace game
