#pragma once

#include <SFML/Audio/Sound.hpp>

#include "engine/Component.h"
#include "engine/GameObject.h"
#include "engine/RenderComponent.h"
#include "engine/Vector2.h"
#include "game/GameState.h"

namespace game {

struct BoardProperties {
    int numTiles{};       /// number of tiles in one row/column of the board
    int screenTileSize{}; /// size of each tile in actual pixels on the screen (including scale of
                          /// parent), assumes square tiles
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
    int _textureTileSize{}; /// size of the board tiles in pixels, hardcoded in start() for now,
                            /// could be made configurable
    Turn _gameState{Turn::Player1Placement}; /// current state of the game

    int _workersPerPlayer{};
    std::vector<WorkerData> _players;
    std::map<std::pair<int, int>, TileData> _tileData{};
    engine::GameObject *_activePlayerLabel{nullptr};
    engine::GameObject *_gameStateLabel{nullptr};
    engine::GameObject *_restartLabel{nullptr};
    WorkerData *_selectedWorker{nullptr};

    void computeBoardBounds();
    void createPlayers();
    void createBoard();
    void setupLabels();
    void updateLabels();
    void moveToTile(engine::GameObject *player, int x, int y);
    bool isTileAdjacentToPlayer(int x, int y);

    /// Check if the player can build on the tile at (x, y)
    ///
    /// Rules for building:
    /// - Must be adjacent to the player's current position (including diagonals)
    /// - Cannot build on a tile with a dome
    /// - Cannot build on a tile occupied by either player
    bool canPlayerBuild(int x, int y);

    /// Check if the player can move to the tile at (x, y)
    ///
    /// Rules for movement:
    /// - Must be adjacent to the player's current position (including diagonals)
    /// - Cannot move onto a tile with a dome
    /// - Cannot move onto a tile occupied by the other player
    /// - Cannot move onto a tile with a building level difference of +2 or more, down is allowed
    bool canPlayerMove(int x, int y);
    void tryMovePlayer(int x, int y);
    bool isGameWon();
    void placeWorker(int playerNumber, int x, int y);
    void placeBuilding(int x, int y);
    void onHoverOvertile(int x, int y);
    void progressState();
    void onClickTile(int x, int y);
    void trySetBuilding(int x, int y);
    void highlightPossibleActions();
    void restartGame();
    void selectWorker(int playerNumber, int x, int y);

    std::tuple<bool, int, int> getTileUnderMouse(int mouseX, int mouseY);
};
} // namespace game
