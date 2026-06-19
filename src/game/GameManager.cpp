#include "game/GameManager.h"
#include "engine/Core.h"
#include "engine/RenderComponent.h"
#include "engine/Vector2.h"
#include <SFML/Window/Keyboard.hpp>

namespace game {

GameManager::GameManager(int workersPerPlayer, int numTiles)
    : _textureTileSize(32), _boardProperties({
                                .numTiles = numTiles,
                                .workersPerPlayer = workersPerPlayer,
                            }),
      _turnSystem(_gameStateData, _boardProperties), _gameView(_gameStateData, _boardProperties) {
    if (numTiles % 2 == 0) {
        throw std::runtime_error("Number of tiles must be odd to have a center tile");
    }
}

void GameManager::start() {
    _boardProperties.screenTileSize = _textureTileSize * owner->localTransform.scale.x;
    createBoard();
    createPlayers();
    computeBoardBounds();
    _gameView.setupLabels(owner);
    _gameView.updateLabels();
}

void GameManager::createBoard() {
    float spacing = _boardProperties.screenTileSize;
    auto tiles = std::make_shared<engine::Texture>("assets/textures/tiles.png");

    auto tilesPerSide = _boardProperties.numTiles /
                        2; // number of tiles from center to edge, e.g. 2 for a 5x5 board

    for (int i = -tilesPerSide; i <= tilesPerSide; ++i) {
        for (int j = -tilesPerSide; j <= tilesPerSide; ++j) {
            auto tile = std::make_unique<engine::GameObject>();
            _gameStateData.tileData[{i, j}].tileObject = tile.get();
            tile->localTransform.position = {.x = i * spacing, .y = j * spacing};
            auto renderComponent = tile->addComponent<engine::RenderComponent>(tiles);
            renderComponent->setTextureRect(
                {.left = 0, .top = 0, .width = _textureTileSize, .height = _textureTileSize});
            owner->addChild(std::move(tile));
        }
    }
};

void GameManager::createPlayers() {
    auto playerTexture = std::make_shared<engine::Texture>("assets/textures/rogues.png");
    for (int i = 0; i < _boardProperties.workersPerPlayer; ++i) {
        auto player1 = std::make_unique<engine::GameObject>();
        player1->enabled = false; // disable player until they are placed on the board
        auto player1RenderComponent =
            player1->addComponent<engine::RenderComponent>(playerTexture, 10);
        player1RenderComponent->setTextureRect({.left = 0,
                                                .top = _textureTileSize * 2,
                                                .width = _textureTileSize,
                                                .height = _textureTileSize});

        _gameStateData.workers.push_back(
            {.position = {}, .object = player1.get(), .playerNumber = 1});
        owner->addChild(std::move(player1));

        auto player2 = std::make_unique<engine::GameObject>();
        player2->enabled = false; // disable player until they are placed on the board
        auto player2RenderComponent =
            player2->addComponent<engine::RenderComponent>(playerTexture, 10);
        player2RenderComponent->setTextureRect({.left = _textureTileSize,
                                                .top = _textureTileSize * 2,
                                                .width = _textureTileSize,
                                                .height = _textureTileSize});
        _gameStateData.workers.push_back(
            {.position = {}, .object = player2.get(), .playerNumber = 2});
        owner->addChild(std::move(player2));
    }
    _gameStateData.workers[0].isSelected = true; // default to player 1 selected at start of game
}

void GameManager::computeBoardBounds() {
    auto [windowWidth, windowHeight] = owner->getCore()->getContext().window->getSize();
    // object is centered at (windowWidth / 2, windowHeight / 2), so the top left corner is at
    auto boardPosition = owner->getWorldTransform().position;
    auto halfBoardSize = (_boardProperties.numTiles / 2.0f) * _boardProperties.screenTileSize;
    _boardProperties.boardTopLeft = {boardPosition.x - halfBoardSize,
                                     boardPosition.y - halfBoardSize};
    ;
    _boardProperties.boardBottomRight = {boardPosition.x + halfBoardSize,
                                         boardPosition.y + halfBoardSize};
    ;
}

std::unique_ptr<engine::Component> GameManager::clone() const {
    return std::make_unique<GameManager>(_boardProperties.workersPerPlayer,
                                         _boardProperties.numTiles);
}

void GameManager::handleEvent(const std::optional<sf::Event> &event, float deltaTime) {
    if (const auto *mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
        auto [hoversInBounds, tileX, tileY] =
            getTileUnderMouse(mouseMoved->position.x, mouseMoved->position.y);
        if (hoversInBounds) {
            onHoverOvertile(tileX, tileY);
        }
    } else if (const auto *mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
        auto [clickedInBounds, tileX, tileY] =
            getTileUnderMouse(mouseButtonPressed->position.x, mouseButtonPressed->position.y);
        if (clickedInBounds) {
            onClickTile(tileX, tileY);
            _turnSystem.updatePossibleActions();
            _gameView.highlightPossibleActions();
            _gameView.updateLabels();
            _gameView.updateBoard();
        }
    } else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::R &&
            (_gameStateData.turn == Turn::Player1Win || _gameStateData.turn == Turn::Player2Win)) {
            restartGame();
        }
    }
}

std::tuple<bool, int, int> GameManager::getTileUnderMouse(int mouseX, int mouseY) {
    if (mouseX >= _boardProperties.boardTopLeft.x &&
        mouseX <= _boardProperties.boardBottomRight.x &&
        mouseY >= _boardProperties.boardTopLeft.y &&
        mouseY <= _boardProperties.boardBottomRight.y) {
        int tileX = static_cast<int>((mouseX - _boardProperties.boardTopLeft.x) /
                                     _boardProperties.screenTileSize) -
                    _boardProperties.numTiles / 2;
        int tileY = static_cast<int>((mouseY - _boardProperties.boardTopLeft.y) /
                                     _boardProperties.screenTileSize) -
                    _boardProperties.numTiles / 2;
        return {true, tileX, tileY};
    }
    return {false, 0, 0};
}

/// Methods to handle hovering and clicking on tiles, top left corner of the board is (0, 0)
void GameManager::onHoverOvertile(int x, int y) {
    // std::cout << "Hovering over tile (" << x << ", " << y << ")\n";
}

/// Forward clicked tile positions to the appropriate handler based on the current game state
void GameManager::onClickTile(int x, int y) {
    if (_gameStateData.turn == Turn::Player1Select) {
        _turnSystem.selectWorker(1, x, y);
    } else if (_gameStateData.turn == Turn::Player2Select) {
        _turnSystem.selectWorker(2, x, y);
    } else if (_gameStateData.turn == Turn::Player1Placement) {
        _turnSystem.placeWorker(1, x, y);
    } else if (_gameStateData.turn == Turn::Player2Placement) {
        _turnSystem.placeWorker(2, x, y);
    } else if (_gameStateData.turn == Turn::Player1Movement) {
        _turnSystem.tryMovePlayer(x, y);
    } else if (_gameStateData.turn == Turn::Player2Movement) {
        _turnSystem.tryMovePlayer(x, y);
    } else if (_gameStateData.turn == Turn::Player1Build) {
        _turnSystem.trySetBuilding(x, y);
    } else if (_gameStateData.turn == Turn::Player2Build) {
        _turnSystem.trySetBuilding(x, y);
    }
}

void GameManager::restartGame() {
    _turnSystem.resetGame();
    _gameView.updateLabels();
    _gameView.updateBoard();
}
const BoardProperties &GameManager::getBoardProperties() const { return _boardProperties; }

} // namespace game