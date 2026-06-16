#pragma once

#include <SFML/Audio/Sound.hpp>
#include <map>

#include "engine/Component.h"
#include "engine/GameObject.h"
#include "engine/RenderComponent.h"
#include "engine/Vector2.h"

namespace game {

enum class GameState {
    Player1Placement,
    Player2Placement,
    Player1Movement,
    Player2Movement,
    Player1Build,
    Player2Build,
    Player1Win,
    Player2Win
};

enum class BuildingLevel { None, Level1, Level2, Level3, Dome }; // move 11 down, then right

enum class HighlightType { None, CanMove, CanBuild, BlockedMove, BlockedBuild };

struct TileData {
    engine::GameObject *tileObject{nullptr};
    BuildingLevel buildingLevel{BuildingLevel::None};
    HighlightType highlight{HighlightType::None};
};

// GameManager is responsible for managing the game state, including the game board, player, and
// other game objects.
// Note: Assumes a square board for simplicity
class GameManager : public engine::Component {
  public:
    GameManager(int numTiles);
    void start() override;
    void handleEvent(const std::optional<sf::Event> &event, float deltaTime) override;

    [[nodiscard]] std::unique_ptr<engine::Component> clone() const override;

  private:
    int _numTiles{};        /// number of tiles in one row/column of the board
    int _screenTileSize{};  /// size of each tile in actual pixels on the screen (including scale of
                            /// parent), assumes square tiles
    int _textureTileSize{}; /// size of the board tiles in pixels, hardcoded in start() for now,
                            /// could be made configurable
    engine::Vector2
        _boardTopLeft{}; /// position of the top left corner of the board in window coordinates
    engine::Vector2 _boardBottomRight{}; /// position of the bottom right corner of the board in
                                         /// window coordinates
    GameState _gameState{GameState::Player1Placement}; /// current state of the game

    engine::GameObject *_player1{nullptr};
    engine::GameObject *_player2{nullptr};
    std::pair<int, int> _player1Position{}; /// current position of player 1 in tile coordinates
    std::pair<int, int> _player2Position{}; /// current position of player 2 in tile coordinates
    std::map<std::pair<int, int>, TileData> _tileData{};

    void computeBoardBounds();
    void createPlayers();
    void createBoard();
    void moveToTile(engine::GameObject *player, int x, int y);
    bool isTileAdjacentToPlayer(int playerNumber, int x, int y);

    /// Check if the player can build on the tile at (x, y)
    ///
    /// Rules for building:
    /// - Must be adjacent to the player's current position (including diagonals)
    /// - Cannot build on a tile with a dome
    /// - Cannot build on a tile occupied by either player
    bool canPlayerBuild(int playerNumber, int x, int y);

    /// Check if the player can move to the tile at (x, y)
    ///
    /// Rules for movement:
    /// - Must be adjacent to the player's current position (including diagonals)
    /// - Cannot move onto a tile with a dome
    /// - Cannot move onto a tile occupied by the other player
    /// - Cannot move onto a tile with a building level difference of +2 or more, down is allowed

    bool canPlayerMove(int playerNumber, int x, int y);
    void tryMovePlayer(int playerNumber, int x, int y);
    bool isGameWon(int playerNumber);
    void placePlayerMove(int playerNumber, int x, int y);
    void placeBuilding(int playerNumber, int x, int y);
    void onHoverOvertile(int x, int y);
    void progressState();
    void onClickTile(int x, int y);
    void trySetBuilding(int playerNumber, int x, int y);
    void highlightPossibleActions();

    std::tuple<bool, int, int> getTileUnderMouse(int mouseX, int mouseY);
};
} // namespace game
