#pragma once

#include <SFML/Audio/Sound.hpp>
#include <map>

#include "engine/Component.h"
#include "engine/Core.h"
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

// GameManager is responsible for managing the game state, including the game board, player, and
// other game objects.
// Note: Assumes a square board for simplicity
class GameManager : public engine::Component {
  public:
    GameManager(int numTiles, int tileSize) : _numTiles(numTiles), _tileSize(tileSize) {}

    void start() override {
        auto [windowWidth, windowHeight] = owner->getCore()->getContext().window->getSize();
        // object is centered at (windowWidth / 2, windowHeight / 2), so the top left corner is at
        // (-numTiles/2 * tileSize, -numTiles/2 * tileSize)
        _boardTopLeft = {windowWidth / 2 - (_numTiles / 2.0f) * _tileSize,
                         windowHeight / 2 - (_numTiles / 2.0f) * _tileSize};
        _boardBottomRight = {windowWidth / 2 + (_numTiles / 2.0f) * _tileSize,
                             windowHeight / 2 + (_numTiles / 2.0f) * _tileSize};

        for (auto &child : owner->getChildren()) {
            auto position = child->localTransform.position;
            int x = static_cast<int>(position.x / _tileSize);
            int y = static_cast<int>(position.y / _tileSize);
            _board[{x, y}] = child.get();
            _buildingLevels[{x, y}] = BuildingLevel::None; // initialize building levels to None
        }
    }

    void connectPlayer(engine::GameObject *player, int playerNumber) {
        assert(player != nullptr);
        if (playerNumber == 1) {
            player1 = player;
        } else if (playerNumber == 2) {
            player2 = player;
        } else {
            throw std::runtime_error("Invalid player number. Must be 1 or 2.");
        }
    }

    void moveToTile(engine::GameObject *player, int x, int y) {
        auto tile = getTile(x, y);
        if (tile == nullptr) {
            printf("No tile at (%d, %d)\n", x, y);
            return;
        }
        player->localTransform.position = {tile->localTransform.position.x,
                                           tile->localTransform.position.y};
    }

    void handleEvent(const std::optional<sf::Event> &event, float deltaTime) override {
        if (const auto *mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
            auto mouseX = mouseMoved->position.x;
            auto mouseY = mouseMoved->position.y;
            // check if the mouse is in the bounds of the board
            if (mouseX >= _boardTopLeft.x && mouseX <= _boardBottomRight.x &&
                mouseY >= _boardTopLeft.y && mouseY <= _boardBottomRight.y) {
                int tileX = static_cast<int>((mouseX - _boardTopLeft.x) / _tileSize);
                int tileY = static_cast<int>((mouseY - _boardTopLeft.y) / _tileSize);
                onHoverOvertile(tileX, tileY);
            }
        } else if (const auto *mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            auto mouseX = mouseButtonPressed->position.x;
            auto mouseY = mouseButtonPressed->position.y;
            // check if the mouse is in the bounds of the board
            if (mouseX >= _boardTopLeft.x && mouseX <= _boardBottomRight.x &&
                mouseY >= _boardTopLeft.y && mouseY <= _boardBottomRight.y) {
                int tileX = static_cast<int>((mouseX - _boardTopLeft.x) / _tileSize);
                int tileY = static_cast<int>((mouseY - _boardTopLeft.y) / _tileSize);
                onClickTile(tileX, tileY);
            }
        }
    }

    /// Methods to handle hovering and clicking on tiles, top left corner of the board is (0, 0)
    void onHoverOvertile(int x, int y) {
        // std::cout << "Hovering over tile (" << x << ", " << y << ")\n";
    }

    [[nodiscard]] int clickToGrid(int coord) const { return coord - _numTiles / 2; }

    void progressState() {
        switch (_gameState) {
        case GameState::Player1Placement:
            _gameState = GameState::Player2Placement;
            break;
        case GameState::Player2Placement:
            _gameState = GameState::Player1Movement;
            break;
        case GameState::Player1Movement:
            _gameState = GameState::Player1Build;
            break;
        case GameState::Player1Build:
            _gameState = GameState::Player2Movement;
            break;
        case GameState::Player2Movement:
            _gameState = GameState::Player2Build;
            break;
        case GameState::Player2Build:
            _gameState = GameState::Player1Movement; // loop back to movement phase
            break;
        default:
            break; // do nothing in win states
        }
    }

    /// Example method to handle clicking on a tile, top left corner of the board is (0, 0)
    void onClickTile(int x, int y) {
        if (_gameState == GameState::Player1Placement) {
            printf("Player 1 placing at (%d, %d)\n", x, y);
            placePlayerMove(1, x, y);
        } else if (_gameState == GameState::Player2Placement) {
            printf("Player 2 placing at (%d, %d)\n", x, y);
            placePlayerMove(2, x, y);
        } else if (_gameState == GameState::Player1Movement) {
            printf("Player 1 trying to move to (%d, %d)\n", x, y);
            tryMovePlayer(1, x, y);
        } else if (_gameState == GameState::Player2Movement) {
            printf("Player 2 trying to move to (%d, %d)\n", x, y);
            tryMovePlayer(2, x, y);
        } else if (_gameState == GameState::Player1Build) {
            printf("Player 1 trying to build at (%d, %d)\n", x, y);
            trySetBuilding(1, x, y);
        } else if (_gameState == GameState::Player2Build) {
            printf("Player 2 trying to build at (%d, %d)\n", x, y);
            trySetBuilding(2, x, y);
        }
    }

    void trySetBuilding(int playerNumber, int x, int y) {
        printf("Player %d trying to build at (%d, %d)\n", playerNumber, x, y);
        if (canPlayerBuild(playerNumber, x, y)) {
            placeBuilding(playerNumber, x, y);
        }
    }

    void placeBuilding(int playerNumber, int x, int y) {
        auto currentLevel = _buildingLevels[{clickToGrid(x), clickToGrid(y)}];
        auto tile = getTile(x, y);
        if (tile == nullptr) {
            printf("No tile at (%d, %d)\n", x, y);
            return;
        }
        auto rc = tile->getComponent<engine::RenderComponent>();
        if (rc != nullptr) {
            auto previousRect = rc->getTextureRect();
            if (currentLevel == BuildingLevel::None) {
                previousRect.top += 11 * 32;
            } else {
                previousRect.left += 32;
            }
            rc->setTextureRect(previousRect);
        }
        _buildingLevels[{clickToGrid(x), clickToGrid(y)}] =
            static_cast<BuildingLevel>(static_cast<int>(currentLevel) + 1);
        progressState();
    }

    void placePlayerMove(int playerNumber, int x, int y) {
        assert(player1 != nullptr &&
               player2 != nullptr); // players should be connected after start, not before
        if (playerNumber == 1) {
            moveToTile(player1, x, y);
            _player1Position = {clickToGrid(x), clickToGrid(y)};
        } else if (playerNumber == 2) {
            moveToTile(player2, x, y);
            _player2Position = {clickToGrid(x), clickToGrid(y)};
        } else {
            throw std::runtime_error("Invalid player number. Must be 1 or 2.");
        }
        progressState();
    }

    void tryMovePlayer(int playerNumber, int x, int y) {
        if (canPlayerMove(playerNumber, x, y)) {
            placePlayerMove(playerNumber, x, y);
        }
    }

    /// Check if the player can move to the tile at (x, y)
    ///
    /// Rules for movement:
    /// - Must be adjacent to the player's current position (including diagonals)
    /// - Cannot move onto a tile with a dome
    /// - Cannot move onto a tile occupied by the other player
    /// - Cannot move onto a tile with a building level difference of +2 or more, down is allowed
    bool canPlayerMove(int playerNumber, int x, int y) {
        bool adjacentToPlayer = isTileAdjacentToPlayer(playerNumber, x, y);
        if (!adjacentToPlayer) {
            return false;
        }
        bool occupiedByPlayer1 =
            clickToGrid(x) == _player1Position.first && clickToGrid(y) == _player1Position.second;
        if (occupiedByPlayer1) {
            return false;
        }
        bool occupiedByPlayer2 =
            clickToGrid(x) == _player2Position.first && clickToGrid(y) == _player2Position.second;
        if (occupiedByPlayer2) {
            return false;
        }
        auto targetLevel = _buildingLevels[{clickToGrid(x), clickToGrid(y)}];
        bool hasDome = targetLevel == BuildingLevel::Dome;
        if (hasDome) {
            return false;
        }
        BuildingLevel currentLevel =
            playerNumber == 1 ? _buildingLevels[{_player1Position.first, _player1Position.second}]
                              : _buildingLevels[{_player2Position.first, _player2Position.second}];
        if (static_cast<int>(targetLevel) - static_cast<int>(currentLevel) > 1) {
            printf("Cannot move to (%d, %d) due to building level difference (%d)\n", x, y,
                   static_cast<int>(targetLevel) - static_cast<int>(currentLevel));
            return false; // cannot move up more than one level
        }
        return true;
    }

    /// Check if the player can build on the tile at (x, y)
    ///
    /// Rules for building:
    /// - Must be adjacent to the player's current position (including diagonals)
    /// - Cannot build on a tile with a dome
    /// - Cannot build on a tile occupied by either player
    bool canPlayerBuild(int playerNumber, int x, int y) {
        bool occupiedByPlayer1 =
            clickToGrid(x) == _player1Position.first && clickToGrid(y) == _player1Position.second;
        bool occupiedByPlayer2 =
            clickToGrid(x) == _player2Position.first && clickToGrid(y) == _player2Position.second;
        return isTileAdjacentToPlayer(playerNumber, x, y) &&
               _buildingLevels[{clickToGrid(x), clickToGrid(y)}] != BuildingLevel::Dome &&
               !occupiedByPlayer1 && !occupiedByPlayer2;
    }

    bool isTileAdjacentToPlayer(int playerNumber, int x, int y) {
        std::pair<int, int> playerPos = (playerNumber == 1) ? _player1Position : _player2Position;
        int dx = std::abs(playerPos.first - clickToGrid(x));
        int dy = std::abs(playerPos.second - clickToGrid(y));
        return (dx <= 1 && dy <= 1) &&
               !(dx == 0 && dy == 0); // adjacent if within 1 tile and not the same tile
    }

    [[nodiscard]] std::unique_ptr<engine::Component> clone() const override {
        return std::make_unique<GameManager>(_numTiles, _tileSize);
    }

    // Example method to get a GameObject at a specific board position
    engine::GameObject *getTile(int x, int y) {
        auto it = _board.find({clickToGrid(x), clickToGrid(y)});
        if (it != _board.end()) {
            return it->second;
        }
        return nullptr; // No tile at this position
    }

  private:
    std::map<std::pair<int, int>, engine::GameObject *>
        _board{};    /// maps tile coordinates (x, y) to GameObjects, where (0, 0) is the top left
                     /// corner of the board
    int _numTiles{}; /// number of tiles in one row/column of the board
    int _tileSize{}; /// size of each tile in pixels, assumes square tiles
    engine::Vector2
        _boardTopLeft{}; /// position of the top left corner of the board in window coordinates
    engine::Vector2 _boardBottomRight{}; /// position of the bottom right corner of the board in
                                         /// window coordinates
    GameState _gameState{GameState::Player1Placement}; /// current state of the game

    engine::GameObject *player1{nullptr};
    engine::GameObject *player2{nullptr};
    std::pair<int, int> _player1Position{}; /// current position of player 1 in tile coordinates
    std::pair<int, int> _player2Position{}; /// current position of player 2 in tile coordinates
    std::map<std::pair<int, int>, BuildingLevel> _buildingLevels{};
};
} // namespace game
