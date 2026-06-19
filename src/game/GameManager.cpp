#include "game/GameManager.h"
#include "engine/Core.h"
#include "engine/Vector2.h"
#include <SFML/Window/Keyboard.hpp>

namespace game {

GameManager::GameManager(int workersPerPlayer, int numTiles)
    : _textureTileSize(32), _boardProperties({
                                .numTiles = numTiles,
                                .workersPerPlayer = workersPerPlayer,
                            }),
      _turnSystem(_gameStateData, _boardProperties) {
    if (numTiles % 2 == 0) {
        throw std::runtime_error("Number of tiles must be odd to have a center tile");
    }
}

void GameManager::start() {
    _boardProperties.screenTileSize = _textureTileSize * owner->localTransform.scale.x;
    createBoard();
    createPlayers();
    computeBoardBounds();
    setupLabels();
    updateLabels();
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
            highlightPossibleActions();
            updateLabels();
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

void GameManager::highlightPossibleActions() {
    // Clear all highlights first
    for (auto &[_, data] : _gameStateData.tileData) {
        data.highlight = HighlightType::None;
    }
    switch (_gameStateData.turn) {
    case Turn::Player1Movement:
        for (auto &[tilePos, data] : _gameStateData.tileData) {
            if (_turnSystem.canPlayerMove(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanMove;
            } else if (_turnSystem.isTileAdjacentToPlayer(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::BlockedMove;
            }
        }
        break;
    case Turn::Player1Build:
        for (auto &[tilePos, data] : _gameStateData.tileData) {
            if (_turnSystem.canPlayerBuild(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanBuild;
            } else if (_turnSystem.isTileAdjacentToPlayer(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::BlockedBuild;
            }
        }
        break;
    case Turn::Player2Movement:
        for (auto &[tilePos, data] : _gameStateData.tileData) {
            if (_turnSystem.canPlayerMove(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanMove;
            } else if (_turnSystem.isTileAdjacentToPlayer(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::BlockedMove;
            }
        }
        break;
    case Turn::Player2Build:
        for (auto &[tilePos, data] : _gameStateData.tileData) {
            if (_turnSystem.canPlayerBuild(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanBuild;
            } else if (_turnSystem.isTileAdjacentToPlayer(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::BlockedBuild;
            }
        }
        break;
    default:
        break; // do nothing in placement and win states
    }
    for (auto &[_, data] : _gameStateData.tileData) {
        switch (data.highlight) {
        case HighlightType::CanMove:
            data.tileObject->getComponent<engine::RenderComponent>()->setTint({150, 150, 255});
            break;
        case HighlightType::CanBuild:
            data.tileObject->getComponent<engine::RenderComponent>()->setTint({150, 255, 150});
            break;
        case HighlightType::BlockedMove:
        case HighlightType::BlockedBuild:
            data.tileObject->getComponent<engine::RenderComponent>()->setTint({255, 150, 150});
            break;
        case HighlightType::None:
            data.tileObject->getComponent<engine::RenderComponent>()->setTint({255, 255, 255});
            break;
        }
    }
}

void GameManager::setupLabels() {
    auto textTexture = std::make_shared<engine::Texture>("assets/textures/text.png");
    auto activePlayerLabel = std::make_unique<engine::GameObject>();
    activePlayerLabel->localTransform.position = {0, (float)(_boardProperties.numTiles / 2 + 1) *
                                                         _boardProperties.screenTileSize};
    activePlayerLabel->localTransform.scale = {0.5f, 0.5f};
    auto activePlayerLabelRenderComponent =
        activePlayerLabel->addComponent<engine::RenderComponent>(
            textTexture, 10, 0, engine::Vector2{.x = 1.0f, .y = 0.0f});
    activePlayerLabelRenderComponent->setTextureRect(
        {.left = 0, .top = 0, .width = 150, .height = 20});
    _activePlayerLabel = activePlayerLabel.get();
    owner->addChild(std::move(activePlayerLabel));

    auto gameStateLabel = std::make_unique<engine::GameObject>();
    gameStateLabel->localTransform.position = {0, (float)(_boardProperties.numTiles / 2 + 1) *
                                                      _boardProperties.screenTileSize};
    gameStateLabel->localTransform.scale = {0.5f, 0.5f};
    auto gameStateLabelRenderComponent = gameStateLabel->addComponent<engine::RenderComponent>(
        textTexture, 10, 0, engine::Vector2{.x = 0.0f, .y = 0.0f});
    gameStateLabelRenderComponent->setTextureRect(
        {.left = 0, .top = 24 * 2, .width = 300, .height = 20});
    _gameStateLabel = gameStateLabel.get();
    owner->addChild(std::move(gameStateLabel));

    auto restartLabel = std::make_unique<engine::GameObject>();
    restartLabel->enabled = false; // only show restart label in win states
    restartLabel->localTransform.position = {0, (float)(_boardProperties.numTiles / 2 + 2) *
                                                    _boardProperties.screenTileSize};
    restartLabel->localTransform.scale = {0.5f, 0.5f};
    auto restartLabelRenderComponent = restartLabel->addComponent<engine::RenderComponent>(
        textTexture, 10, 0, engine::Vector2{.x = 0.0f, .y = 0.0f});
    restartLabelRenderComponent->setTextureRect(
        {.left = 0, .top = 24 * 7, .width = 300, .height = 20});
    _restartLabel = restartLabel.get();
    owner->addChild(std::move(restartLabel));
}
void GameManager::updateLabels() {
    auto player1turnRect = engine::Rect{.left = 0, .top = 0, .width = 150, .height = 20};
    auto player2turnRect = engine::Rect{.left = 0, .top = 24 * 1, .width = 150, .height = 20};
    auto selectRect = engine::Rect{.left = 0, .top = 24 * 2, .width = 300, .height = 20};
    auto placeRect = engine::Rect{.left = 0, .top = 24 * 3, .width = 300, .height = 20};
    auto moveRect = engine::Rect{.left = 0, .top = 24 * 4, .width = 300, .height = 20};
    auto buildRect = engine::Rect{.left = 0, .top = 24 * 5, .width = 300, .height = 20};
    auto winnerRect = engine::Rect{.left = 0, .top = 24 * 6, .width = 300, .height = 20};
    switch (_gameStateData.turn) {
    case Turn::Player1Placement:
    case Turn::Player1Movement:
    case Turn::Player1Build:
    case Turn::Player1Win:
    case Turn::Player1Select:
        _activePlayerLabel->getComponent<engine::RenderComponent>()->setTextureRect(
            player1turnRect);
        break;
    default:
        _activePlayerLabel->getComponent<engine::RenderComponent>()->setTextureRect(
            player2turnRect);
        break;
    }

    switch (_gameStateData.turn) {
    case Turn::Player1Placement:
    case Turn::Player2Placement:
        _restartLabel->enabled = false;
        _gameStateLabel->getComponent<engine::RenderComponent>()->setTextureRect(placeRect);
        break;
    case Turn::Player1Movement:
    case Turn::Player2Movement:
        _gameStateLabel->getComponent<engine::RenderComponent>()->setTextureRect(moveRect);
        break;
    case Turn::Player1Build:
    case Turn::Player2Build:
        _gameStateLabel->getComponent<engine::RenderComponent>()->setTextureRect(buildRect);
        break;
    case Turn::Player1Win:
    case Turn::Player2Win:
        _restartLabel->enabled = true;
        _gameStateLabel->getComponent<engine::RenderComponent>()->setTextureRect(winnerRect);
        break;
    case Turn::Player1Select:
    case Turn::Player2Select:
        _gameStateLabel->getComponent<engine::RenderComponent>()->setTextureRect(selectRect);
        break;
    }
}

void GameManager::restartGame() {
    // Reset game state
    _gameStateData.turn = Turn::Player1Placement;
    for (auto &workerData : _gameStateData.workers) {
        workerData.isSelected = false;
        workerData.position = {0, 0};
        workerData.object->enabled = false;
    }
    // default select the first worker of player 1 at the start of the game
    _gameStateData.workers[0].isSelected = true;

    // Reset tile data and visuals
    for (auto &[tilePos, data] : _gameStateData.tileData) {
        data.buildingLevel = BuildingLevel::None;
        data.highlight = HighlightType::None;
        auto rc = data.tileObject->getComponent<engine::RenderComponent>();
        if (rc != nullptr) {
            rc->setTextureRect(
                {.left = 0, .top = 0, .width = _textureTileSize, .height = _textureTileSize});
            rc->setTint({255, 255, 255});
        }
    }
    updateLabels();
}
const BoardProperties &GameManager::getBoardProperties() const { return _boardProperties; }

} // namespace game