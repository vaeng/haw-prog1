#pragma once

#include <SFML/Audio/Sound.hpp>
#include <map>

#include "engine/Component.h"
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
        // Assumes the board is centered at (0, 0) and extends from (-numTiles/2 * tileSize,
        // -numTiles/2 * tileSize) to (numTiles/2 * tileSize, numTiles/2 * tileSize)
        // Also assumes, that the GameObjects are already created and added to the scene as children
        // to the owning object, so we just need to populate the _board map
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
        }
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
    std::map<std::pair<int, int>, engine::GameObject *> _board{}; // maps (x, y) to GameObject
    int _numTiles{};
    int _tileSize{};
    engine::Vector2 _boardTopLeft{};
    engine::Vector2 _boardBottomRight{};
};
} // namespace game
