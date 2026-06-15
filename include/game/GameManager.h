#pragma once

#include <SFML/Audio/Sound.hpp>
#include <map>

#include "engine/Component.h"
#include "engine/Core.h"
#include "engine/GameObject.h"

#include <iostream>

namespace game {

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
        }
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

    /// Example method to handle clicking on a tile, top left corner of the board is (0, 0)
    void onClickTile(int x, int y) {
        // std::cout << "Clicking tile (" << x << ", " << y << ")\n";
    }

    [[nodiscard]] std::unique_ptr<engine::Component> clone() const override {
        return std::make_unique<GameManager>(_numTiles, _tileSize);
    }

    // Example method to get a GameObject at a specific board position
    engine::GameObject *getTile(int x, int y) {
        auto it = _board.find({x, y});
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
};
} // namespace game
