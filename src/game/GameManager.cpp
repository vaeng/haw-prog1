#include "game/GameManager.h"
#include "engine/Core.h"
#include "engine/Vector2.h"
#include <SFML/Window/Keyboard.hpp>

namespace game {

GameManager::GameManager(int workersPerPlayer, int numTiles)
    : _workersPerPlayer(workersPerPlayer), _boardProperties({.numTiles = numTiles}) {
    if (numTiles % 2 == 0) {
        throw std::runtime_error("Number of tiles must be odd to have a center tile");
    }
}

void GameManager::start() {
    _textureTileSize = 32; // depends on texture used for the board, hardcoded for now, could be
                           // made configurable
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
            _tileData[{i, j}].tileObject = tile.get();
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
    for (int i = 0; i < _workersPerPlayer; ++i) {
        auto player1 = std::make_unique<engine::GameObject>();
        player1->enabled = false; // disable player until they are placed on the board
        auto player1RenderComponent =
            player1->addComponent<engine::RenderComponent>(playerTexture, 10);
        player1RenderComponent->setTextureRect({.left = 0,
                                                .top = _textureTileSize * 2,
                                                .width = _textureTileSize,
                                                .height = _textureTileSize});

        _players.push_back({.position = {}, .object = player1.get(), .number = 1});
        owner->addChild(std::move(player1));

        auto player2 = std::make_unique<engine::GameObject>();
        player2->enabled = false; // disable player until they are placed on the board
        auto player2RenderComponent =
            player2->addComponent<engine::RenderComponent>(playerTexture, 10);
        player2RenderComponent->setTextureRect({.left = _textureTileSize,
                                                .top = _textureTileSize * 2,
                                                .width = _textureTileSize,
                                                .height = _textureTileSize});
        _players.push_back({.position = {}, .object = player2.get(), .number = 2});
        owner->addChild(std::move(player2));
    }
    _selectedWorker = &_players[0]; // default to player 1 selected at start of game
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

void GameManager::moveToTile(engine::GameObject *player, int x, int y) {
    auto tile = _tileData[{x, y}].tileObject;
    if (tile == nullptr) {
        printf("No tile at (%d, %d)\n", x, y);
        return;
    }
    player->localTransform.position = {tile->localTransform.position.x,
                                       tile->localTransform.position.y};
}

bool GameManager::isTileAdjacentToPlayer(int x, int y) {
    auto playerPos = _selectedWorker->position;
    int dx = std::abs(playerPos.first - x);
    int dy = std::abs(playerPos.second - y);
    return (dx <= 1 && dy <= 1) &&
           !(dx == 0 && dy == 0); // adjacent if within 1 tile and not the same tile
}

std::unique_ptr<engine::Component> GameManager::clone() const {
    return std::make_unique<GameManager>(_workersPerPlayer, _boardProperties.numTiles);
}

bool GameManager::canPlayerBuild(int x, int y) {
    bool occupiedByAnyWorker = false;
    for (const auto &playerData : _players) {
        if (x == playerData.position.first && y == playerData.position.second) {
            occupiedByAnyWorker = true;
            break;
        }
    }

    return isTileAdjacentToPlayer(x, y) && _tileData[{x, y}].buildingLevel != BuildingLevel::Dome &&
           !occupiedByAnyWorker;
}

bool GameManager::canPlayerMove(int x, int y) {
    bool adjacentToPlayer = isTileAdjacentToPlayer(x, y);
    if (!adjacentToPlayer) {
        return false;
    }
    bool occupiedByAnyWorker = false;
    for (const auto &playerData : _players) {
        if (x == playerData.position.first && y == playerData.position.second) {
            occupiedByAnyWorker = true;
            break;
        }
    }
    if (occupiedByAnyWorker) {
        return false;
    }
    auto targetLevel = _tileData[{x, y}].buildingLevel;
    bool hasDome = targetLevel == BuildingLevel::Dome;
    if (hasDome) {
        return false;
    }
    BuildingLevel currentLevel =
        _tileData[{_selectedWorker->position.first, _selectedWorker->position.second}]
            .buildingLevel;

    if (static_cast<int>(targetLevel) - static_cast<int>(currentLevel) > 1) {
        return false; // cannot move up more than one level
    }
    return true;
}

void GameManager::tryMovePlayer(int x, int y) {
    if (canPlayerMove(x, y)) {
        _selectedWorker->position = {x, y};
        _selectedWorker->object->enabled = true; // enable player once it is placed on the board
        moveToTile(_selectedWorker->object, x, y);

        if (isGameWon()) {
            _gameState =
                (_selectedWorker->number == 1) ? GameState::Player1Win : GameState::Player2Win;
        } else {
            progressState();
        }
    }
}

bool GameManager::isGameWon() {
    return _tileData[{_selectedWorker->position.first, _selectedWorker->position.second}]
               .buildingLevel == BuildingLevel::Level3;
}

void GameManager::placeWorker(int playerNumber, int x, int y) {
    // check if tile is already occupied by another worker
    for (const auto &playerData : _players) {
        if (x == playerData.position.first && y == playerData.position.second) {
            return; // tile is occupied, do not place worker
        }
    }
    // grab first worker that matches the player number and is not yet placed on the board
    for (auto &playerData : _players) {
        if (playerData.number == playerNumber && playerData.object->enabled == false) {
            playerData.position = {x, y};
            playerData.object->enabled = true; // enable player once it is placed on the board
            moveToTile(playerData.object, x, y);
            break;
        }
    }
    // assuming players place all their workers at once, before the other one starts:
    auto allWorkersPlaced = true;
    for (const auto &playerData : _players) {
        if (!playerData.object->enabled && playerData.number == playerNumber) {
            allWorkersPlaced = false;
            break;
        }
    }
    if (allWorkersPlaced) {
        progressState();
    }
}

void GameManager::placeBuilding(int x, int y) {
    auto currentLevel = _tileData[{x, y}].buildingLevel;
    auto tile = _tileData[{x, y}].tileObject;
    if (tile == nullptr) {
        return;
    }
    auto rc = tile->getComponent<engine::RenderComponent>();
    if (rc != nullptr) {
        auto previousRect = rc->getTextureRect();
        if (currentLevel == BuildingLevel::None) {
            previousRect.top += 11 * _textureTileSize;
        } else {
            previousRect.left += _textureTileSize;
        }
        rc->setTextureRect(previousRect);
    }
    _tileData[{x, y}].buildingLevel =
        static_cast<BuildingLevel>(static_cast<int>(currentLevel) + 1);
    progressState();
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
            (_gameState == GameState::Player1Win || _gameState == GameState::Player2Win)) {
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

void GameManager::progressState() {
    switch (_gameState) {
    case GameState::Player1Placement:
        _gameState = GameState::Player2Placement;
        break;
    case GameState::Player2Placement:
        _gameState = GameState::Player1Select;
        break;
    case GameState::Player1Select:
        _gameState = GameState::Player1Movement;
        break;
    case GameState::Player1Movement:
        _gameState = GameState::Player1Build;
        break;
    case GameState::Player1Build:
        _gameState = GameState::Player2Select;
        break;
    case GameState::Player2Select:
        _gameState = GameState::Player2Movement;
        break;
    case GameState::Player2Movement:
        _gameState = GameState::Player2Build;
        break;
    case GameState::Player2Build:
        _gameState = GameState::Player1Select; // loop back to movement phase
        break;
    default:
        break; // do nothing in win states
    }
}

/// Forward clicked tile positions to the appropriate handler based on the current game state
void GameManager::onClickTile(int x, int y) {
    if (_gameState == GameState::Player1Select) {
        selectWorker(1, x, y);
    } else if (_gameState == GameState::Player2Select) {
        selectWorker(2, x, y);
    } else if (_gameState == GameState::Player1Placement) {
        placeWorker(1, x, y);
    } else if (_gameState == GameState::Player2Placement) {
        placeWorker(2, x, y);
    } else if (_gameState == GameState::Player1Movement) {
        tryMovePlayer(x, y);
    } else if (_gameState == GameState::Player2Movement) {
        tryMovePlayer(x, y);
    } else if (_gameState == GameState::Player1Build) {
        trySetBuilding(x, y);
    } else if (_gameState == GameState::Player2Build) {
        trySetBuilding(x, y);
    }
}

void GameManager::selectWorker(int playerNumber, int x, int y) {
    for (auto &playerData : _players) {
        if (playerData.number == playerNumber && playerData.position.first == x &&
            playerData.position.second == y) {
            _selectedWorker = &playerData;
            progressState();
            break;
        }
    }
}

void GameManager::trySetBuilding(int x, int y) {
    if (canPlayerBuild(x, y)) {
        placeBuilding(x, y);
    }
}

void GameManager::highlightPossibleActions() {
    // Clear all highlights first
    for (auto &[_, data] : _tileData) {
        data.highlight = HighlightType::None;
    }
    switch (_gameState) {
    case GameState::Player1Movement:
        for (auto &[tilePos, data] : _tileData) {
            if (canPlayerMove(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanMove;
            } else if (isTileAdjacentToPlayer(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::BlockedMove;
            }
        }
        break;
    case GameState::Player1Build:
        for (auto &[tilePos, data] : _tileData) {
            if (canPlayerBuild(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanBuild;
            } else if (isTileAdjacentToPlayer(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::BlockedBuild;
            }
        }
        break;
    case GameState::Player2Movement:
        for (auto &[tilePos, data] : _tileData) {
            if (canPlayerMove(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanMove;
            } else if (isTileAdjacentToPlayer(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::BlockedMove;
            }
        }
        break;
    case GameState::Player2Build:
        for (auto &[tilePos, data] : _tileData) {
            if (canPlayerBuild(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanBuild;
            } else if (isTileAdjacentToPlayer(tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::BlockedBuild;
            }
        }
        break;
    default:
        break; // do nothing in placement and win states
    }
    for (auto &[_, data] : _tileData) {
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
    switch (_gameState) {
    case GameState::Player1Placement:
    case GameState::Player1Movement:
    case GameState::Player1Build:
    case GameState::Player1Win:
    case GameState::Player1Select:
        _activePlayerLabel->getComponent<engine::RenderComponent>()->setTextureRect(
            player1turnRect);
        break;
    default:
        _activePlayerLabel->getComponent<engine::RenderComponent>()->setTextureRect(
            player2turnRect);
        break;
    }

    switch (_gameState) {
    case GameState::Player1Placement:
    case GameState::Player2Placement:
        _restartLabel->enabled = false;
        _gameStateLabel->getComponent<engine::RenderComponent>()->setTextureRect(placeRect);
        break;
    case GameState::Player1Movement:
    case GameState::Player2Movement:
        _gameStateLabel->getComponent<engine::RenderComponent>()->setTextureRect(moveRect);
        break;
    case GameState::Player1Build:
    case GameState::Player2Build:
        _gameStateLabel->getComponent<engine::RenderComponent>()->setTextureRect(buildRect);
        break;
    case GameState::Player1Win:
    case GameState::Player2Win:
        _restartLabel->enabled = true;
        _gameStateLabel->getComponent<engine::RenderComponent>()->setTextureRect(winnerRect);
        break;
    case GameState::Player1Select:
    case GameState::Player2Select:
        _gameStateLabel->getComponent<engine::RenderComponent>()->setTextureRect(selectRect);
        break;
    }
}

void GameManager::restartGame() {
    // Reset game state
    _gameState = GameState::Player1Placement;
    _selectedWorker = nullptr;
    for (auto &playerData : _players) {
        playerData.position = {0, 0};
        playerData.object->enabled = false;
    }

    // Reset tile data and visuals
    for (auto &[tilePos, data] : _tileData) {
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