#pragma once

#include "engine/MessageBus.h"
#include "game/GameMessages.h"
#include "game/GameState.h"

namespace game {

class BoardProperties;

class TurnSystem {
  public:
    TurnSystem(GameStateData &gameStateData, const BoardProperties &boardProperties)
        : _gameStateData(gameStateData), _boardProperties(boardProperties) {};

    void setup(engine::MessageBus *bus);

  private:
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
    /// - Cannot move onto a tile with a building level difference of +2 or more, down is
    /// allowed
    bool canPlayerMove(int x, int y);

    bool isTileAdjacentToPlayer(int x, int y);
    bool isGameWon();
    void progressState();
    void resetGame();

    void tryMovePlayer(int x, int y);
    void moveToTile(engine::GameObject *player, int x, int y);
    void placeWorker(int playerNumber, int x, int y);
    void trySetBuilding(int x, int y);
    void placeBuilding(int x, int y);
    void selectWorker(int playerNumber, int x, int y);
    void updatePossibleActions();

    void onTileClicked(TileClickedMessage message);

    WorkerData &getSelectedWorker();
    GameStateData &_gameStateData;
    const BoardProperties &_boardProperties;
    engine::MessageBus *_messageBus{nullptr};
    engine::Connection _tileClickedConnection;
    engine::Connection _gameRestartedConnection;
};

} // namespace game
