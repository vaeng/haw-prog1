#include "game/GameManager.h"
#include "engine/Core.h"

namespace game {

GameManager::GameManager(int numTiles) : _numTiles(numTiles) {
    if (numTiles % 2 == 0) {
        throw std::runtime_error("Number of tiles must be odd to have a center tile");
    }
}

void GameManager::start() {
    _textureTileSize = 32; // depends on texture used for the board, hardcoded for now, could be
                           // made configurable
    _screenTileSize = _textureTileSize * owner->localTransform.scale.x;
    createBoard();
    createPlayers();
    computeBoardBounds();
}

void GameManager::createBoard() {
    float spacing = _screenTileSize;
    auto tiles = std::make_shared<engine::Texture>("assets/textures/tiles.png");

    auto tilesPerSide =
        _numTiles / 2; // number of tiles from center to edge, e.g. 2 for a 5x5 board

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
    auto player1 = std::make_unique<engine::GameObject>();
    player1->enabled = false; // disable player until they are placed on the board
    auto player1RenderComponent = player1->addComponent<engine::RenderComponent>(playerTexture, 10);
    player1RenderComponent->setTextureRect({.left = 0,
                                            .top = _textureTileSize * 2,
                                            .width = _textureTileSize,
                                            .height = _textureTileSize});
    _player1 = player1.get();
    owner->addChild(std::move(player1));

    auto player2 = std::make_unique<engine::GameObject>();
    player2->enabled = false; // disable player until they are placed on the board
    auto player2RenderComponent = player2->addComponent<engine::RenderComponent>(playerTexture, 10);
    player2RenderComponent->setTextureRect({.left = _textureTileSize,
                                            .top = _textureTileSize * 2,
                                            .width = _textureTileSize,
                                            .height = _textureTileSize});
    _player2 = player2.get();
    owner->addChild(std::move(player2));
}

void GameManager::computeBoardBounds() {
    auto [windowWidth, windowHeight] = owner->getCore()->getContext().window->getSize();
    // object is centered at (windowWidth / 2, windowHeight / 2), so the top left corner is at
    auto boardPosition = owner->getWorldTransform().position;
    auto halfBoardSize = (_numTiles / 2.0f) * _screenTileSize;
    _boardTopLeft = {boardPosition.x - halfBoardSize, boardPosition.y - halfBoardSize};
    _boardBottomRight = {boardPosition.x + halfBoardSize, boardPosition.y + halfBoardSize};
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

bool GameManager::isTileAdjacentToPlayer(int playerNumber, int x, int y) {
    std::pair<int, int> playerPos = (playerNumber == 1) ? _player1Position : _player2Position;
    int dx = std::abs(playerPos.first - x);
    int dy = std::abs(playerPos.second - y);
    return (dx <= 1 && dy <= 1) &&
           !(dx == 0 && dy == 0); // adjacent if within 1 tile and not the same tile
}

std::unique_ptr<engine::Component> GameManager::clone() const {
    return std::make_unique<GameManager>(_numTiles);
}

bool GameManager::canPlayerBuild(int playerNumber, int x, int y) {
    bool occupiedByPlayer1 = x == _player1Position.first && y == _player1Position.second;
    bool occupiedByPlayer2 = x == _player2Position.first && y == _player2Position.second;
    return isTileAdjacentToPlayer(playerNumber, x, y) &&
           _tileData[{x, y}].buildingLevel != BuildingLevel::Dome && !occupiedByPlayer1 &&
           !occupiedByPlayer2;
}

bool GameManager::canPlayerMove(int playerNumber, int x, int y) {
    bool adjacentToPlayer = isTileAdjacentToPlayer(playerNumber, x, y);
    if (!adjacentToPlayer) {
        return false;
    }
    bool occupiedByPlayer1 = x == _player1Position.first && y == _player1Position.second;
    if (occupiedByPlayer1) {
        return false;
    }
    bool occupiedByPlayer2 = x == _player2Position.first && y == _player2Position.second;
    if (occupiedByPlayer2) {
        return false;
    }
    auto targetLevel = _tileData[{x, y}].buildingLevel;
    bool hasDome = targetLevel == BuildingLevel::Dome;
    if (hasDome) {
        return false;
    }
    BuildingLevel currentLevel =
        playerNumber == 1
            ? _tileData[{_player1Position.first, _player1Position.second}].buildingLevel
            : _tileData[{_player2Position.first, _player2Position.second}].buildingLevel;
    if (static_cast<int>(targetLevel) - static_cast<int>(currentLevel) > 1) {
        printf("Cannot move to (%d, %d) due to building level difference (%d)\n", x, y,
               static_cast<int>(targetLevel) - static_cast<int>(currentLevel));
        return false; // cannot move up more than one level
    }
    return true;
}

void GameManager::tryMovePlayer(int playerNumber, int x, int y) {
    if (canPlayerMove(playerNumber, x, y)) {
        placePlayerMove(playerNumber, x, y);
    }
}

bool GameManager::isGameWon(int playerNumber) {
    if (playerNumber == 1) {
        return _tileData[{_player1Position.first, _player1Position.second}].buildingLevel ==
               BuildingLevel::Level3;
    } else if (playerNumber == 2) {
        return _tileData[{_player2Position.first, _player2Position.second}].buildingLevel ==
               BuildingLevel::Level3;
    }
    return false;
}

void GameManager::placePlayerMove(int playerNumber, int x, int y) {
    assert(_player1 != nullptr &&
           _player2 != nullptr); // players should be connected after start, not before
    if (playerNumber == 1) {
        _player1->enabled = true; // enable player once it is placed on the board
        moveToTile(_player1, x, y);
        _player1Position = {x, y};
    } else if (playerNumber == 2) {
        _player2->enabled = true; // enable player once it is placed on the board
        moveToTile(_player2, x, y);
        _player2Position = {x, y};
    } else {
        throw std::runtime_error("Invalid player number. Must be 1 or 2.");
    }
    if (isGameWon(playerNumber)) {
        _gameState = (playerNumber == 1) ? GameState::Player1Win : GameState::Player2Win;
        printf("Player %d wins!\n", playerNumber);
    } else {
        progressState();
    }
}

void GameManager::placeBuilding(int playerNumber, int x, int y) {
    auto currentLevel = _tileData[{x, y}].buildingLevel;
    auto tile = _tileData[{x, y}].tileObject;
    if (tile == nullptr) {
        printf("No tile at (%d, %d)\n", x, y);
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
        }
    }
}

std::tuple<bool, int, int> GameManager::getTileUnderMouse(int mouseX, int mouseY) {
    if (mouseX >= _boardTopLeft.x && mouseX <= _boardBottomRight.x && mouseY >= _boardTopLeft.y &&
        mouseY <= _boardBottomRight.y) {
        int tileX = static_cast<int>((mouseX - _boardTopLeft.x) / _screenTileSize) - _numTiles / 2;
        int tileY = static_cast<int>((mouseY - _boardTopLeft.y) / _screenTileSize) - _numTiles / 2;
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

/// Forward clicked tile positions to the appropriate handler based on the current game state
void GameManager::onClickTile(int x, int y) {
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

void GameManager::trySetBuilding(int playerNumber, int x, int y) {
    printf("Player %d trying to build at (%d, %d)\n", playerNumber, x, y);
    if (canPlayerBuild(playerNumber, x, y)) {
        placeBuilding(playerNumber, x, y);
    }
}

void GameManager::highlightPossibleActions() {
    // Clear all highlights first
    for (auto &[_, data] : _tileData) {
        data.highlight = HighlightType::None;
    }
    switch (_gameState) {
    case GameState::Player1Movement:
        printf("Highlighting possible moves for Player 1\n");
        for (auto &[tilePos, data] : _tileData) {
            if (canPlayerMove(1, tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanMove;
            } else if (isTileAdjacentToPlayer(1, tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::BlockedMove;
            }
        }
        break;
    case GameState::Player1Build:
        printf("Highlighting possible builds for Player 1\n");
        for (auto &[tilePos, data] : _tileData) {
            if (canPlayerBuild(1, tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanBuild;
            } else if (isTileAdjacentToPlayer(1, tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::BlockedBuild;
            }
        }
        break;
    case GameState::Player2Movement:
        printf("Highlighting possible moves for Player 2\n");
        for (auto &[tilePos, data] : _tileData) {
            if (canPlayerMove(2, tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanMove;
            } else if (isTileAdjacentToPlayer(2, tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::BlockedMove;
            }
        }
        break;
    case GameState::Player2Build:
        printf("Highlighting possible builds for Player 2\n");
        for (auto &[tilePos, data] : _tileData) {
            if (canPlayerBuild(2, tilePos.first, tilePos.second)) {
                data.highlight = HighlightType::CanBuild;
            } else if (isTileAdjacentToPlayer(2, tilePos.first, tilePos.second)) {
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
} // namespace game