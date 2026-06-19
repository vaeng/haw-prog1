#include "game/GameManager.h"
#include "engine/Core.h"
#include "engine/MessageBus.h"
#include "engine/RenderComponent.h"
#include "engine/Vector2.h"
#include <SFML/Window/Keyboard.hpp>

namespace game {

GameManager::GameManager(int workersPerPlayer, int numTiles)
    : _textureTileSize(32), _boardProperties({
                                .numTiles = numTiles,
                                .workersPerPlayer = workersPerPlayer,
                            }),
      _turnSystem(_gameStateData, _boardProperties), _gameView(_gameStateData, _boardProperties),
      _inputSystem(_boardProperties) {
    if (numTiles % 2 == 0) {
        throw std::runtime_error("Number of tiles must be odd to have a center tile");
    }
}

std::unique_ptr<engine::Component> GameManager::clone() const {
    return std::make_unique<GameManager>(_boardProperties.workersPerPlayer,
                                         _boardProperties.numTiles);
}

/// Translates sfml events into game messages and publishes them to the message bus
void GameManager::handleEvent(const std::optional<sf::Event> &event, float deltaTime) {
    if (const auto *mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
        _messageBus->publish<MouseMovedMessage>(
            {.windowX = mouseMoved->position.x, .windowY = mouseMoved->position.y});
    } else if (const auto *mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
        _messageBus->publish<MouseClickedMessage>(
            {.windowX = mouseButtonPressed->position.x, .windowY = mouseButtonPressed->position.y});
    } else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::R) {
            _messageBus->publish<RestartGameMessage>({});
        }
    }
}

void GameManager::computeBoardBounds() {
    _boardProperties.screenTileSize = _textureTileSize * owner->localTransform.scale.x;
    auto [windowWidth, windowHeight] = owner->getCore()->getContext().window->getSize();
    // object is centered at (windowWidth / 2, windowHeight / 2), so the top left corner is at
    auto boardPosition = owner->getWorldTransform().position;
    auto halfBoardSize = (_boardProperties.numTiles / 2.0f) * _boardProperties.screenTileSize;
    _boardProperties.boardTopLeft = {boardPosition.x - halfBoardSize,
                                     boardPosition.y - halfBoardSize};
    _boardProperties.boardBottomRight = {boardPosition.x + halfBoardSize,
                                         boardPosition.y + halfBoardSize};
}

void GameManager::start() {
    // Compute board properties based on texture size and window size, then set up the game view and
    // input system, which depend on those properties
    computeBoardBounds();
    _messageBus = owner->getCore()->getContext().messageBus;
    _gameView.setup(owner, _messageBus);
    _inputSystem.setup(_messageBus);
    _turnSystem.setup(_messageBus);
}

const BoardProperties &GameManager::getBoardProperties() const { return _boardProperties; }

} // namespace game