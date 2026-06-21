#pragma once

#include "engine/Component.h"
#include "engine/GameObject.h"
#include "engine/MessageBus.h"
#include "engine/Sprite.h"
#include "game/GameState.h"

#include <map>

namespace game {

class GameStateData;
class BoardProperties;

/// GameView is responsible for rendering the game state to the screen
///
/// That includes the game board, players, and other game objects. It listens for changes in the
/// game state and updates the visuals accordingly. It also handles animations for building
/// placements and worker movements.

class GameView {
  public:
    GameView(GameStateData &gameStateData, const BoardProperties &boardProperties)
        : _gameStateData(gameStateData), _boardProperties(boardProperties) {}
    void setup(engine::GameObject *owner, engine::MessageBus *bus);

  private:
    GameStateData &_gameStateData; // reference to the game state data to read from when rendering
    const BoardProperties
        &_boardProperties; // reference to the board properties for rendering calculations
    engine::MessageBus *_messageBus{nullptr};
    engine::Connection _stateChangedConnection;
    engine::Connection _buildingPlacedConnection;
    engine::Connection _workerMovedConnection;
    engine::Connection _workerPlacedConnection;
    engine::Connection _tileHoveredConnection;

    int _textureTileSize{32}; /// size of the board tiles in pixels, hardcoded for now
    int _playerSpriteHeight{48}; // height of player sprites in pixels, hardcoded for now
    engine::GameObject *_activePlayerLabel{nullptr};
    engine::GameObject *_gameStateLabel{nullptr};
    engine::GameObject *_restartLabel{nullptr};

    std::map<std::pair<int, int>, engine::GameObject *>
        _tileObjects; // map of tile positions to their GameObjects for access when updating
                      // highlights and building levels
    std::map<std::pair<int, int>, engine::GameObject *>
        _buildings; // map of building positions to their GameObjects for access when displaying
                    // building
    std::map<int, engine::GameObject *>
        _workerObjects; // list of worker GameObjects, indexed by worker id for access when updating
                        // worker positions
    std::map<BuildingLevel, engine::Rect>
        _buildingTextureRects; // map of building levels to their texture rects in the spritesheet
    engine::GameObject *_player1MovePreview{
        nullptr}; // GameObject used to show a preview of where player 1 is about to move, updated
                  // on tile hover
    engine::GameObject *_player2MovePreview{nullptr}; // same as above but for player 2

    /// Updates the player and game state labels to reflect the current game state
    ///
    /// Called whenever the game state changes, via message bus subscription.
    void updateLabels();

    /// Updates the board according to the building levels in the game state
    ///
    /// Called whenever the game state changes, via message bus subscription
    void updateBoard();

    /// Updates the player positions according to the worker positions in the game state
    ///
    /// Called whenever the game state changes, via message bus subscription
    /// Has some guard clauses to avoid updating positions if the worker/building animations are
    /// still playing, to avoid updates from interrupting the animations
    void updatePlayerPositions();

    /// Tints the the tiles around the selected worker to show where the player can build/move
    void highlightPossibleActions();

    /// Adds labels to the scene for showing the active player and game state
    void setupLabels(engine::GameObject *owner);

    /// Adds the worker GameObjects (and preview objects) to the scene
    ///
    /// Also sets up the objects with RenderComponents and AnimationsComponents as needed, and
    /// stores references to them for later updates
    void createPlayers(engine::GameObject *owner);

    /// Adds the tile and building GameObjects to the scene, according to the board properties
    void createBoard(engine::GameObject *owner);

    /// Helper function to fill the _buildingTextureRects map with the correct texture rects for
    /// each building level
    void fillBuildingTextureRects();

    /// Plays the building placement animation at the given position and level
    void animatedBuildingPlacement(int x, int y, BuildingLevel level);

    /// Plays the worker placement animation for the given worker id
    void animatedWorkerPlacement(int workerId);

    /// Plays the worker movement animation for the given worker id
    ///
    /// Implemented via a queue in the AnimationComponent and a render offset
    void workerMovementAnimation(int workerId, int originX, int originY, int destinationX,
                                 int destinationY);

    /// Shows a preview of the possible move when hovering over a tile for build/move
    void tileHoveredPreview(int x, int y);
};

} // namespace game