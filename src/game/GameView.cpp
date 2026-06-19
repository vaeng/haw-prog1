#include "game/GameView.h"
#include "engine/GameObject.h"
#include "engine/RenderComponent.h"
#include "game/GameManager.h"

namespace game {
void GameView::setup(engine::GameObject *owner, engine::MessageBus *bus) {
    _messageBus = bus;
    createBoard(owner);
    createPlayers(owner);
    setupLabels(owner);
    _stateChangedConnection =
        _messageBus->subscribe<StateChangedMessage>([this](const StateChangedMessage &) {
            updateLabels();
            highlightPossibleActions();
            updateBoard();
            updatePlayerPositions();
        });
}

void GameView::createBoard(engine::GameObject *owner) {
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

void GameView::createPlayers(engine::GameObject *owner) {
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

void GameView::setupLabels(engine::GameObject *owner) {
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

void GameView::updateLabels() {
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

void GameView::highlightPossibleActions() {
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

void GameView::updateBoard() {
    // Reset tile data and visuals
    for (auto &[tilePos, data] : _gameStateData.tileData) {
        auto currentLevel = data.buildingLevel;
        auto tile = data.tileObject;
        if (tile == nullptr) {
            continue;
        }
        auto rc = tile->getComponent<engine::RenderComponent>();
        if (rc != nullptr) {
            auto previousRect = rc->getTextureRect();
            if (currentLevel == BuildingLevel::None) {
                previousRect = {
                    .left = 0, .top = 0, .width = _textureTileSize, .height = _textureTileSize};
            } else {
                previousRect = {.left = _textureTileSize * ((int)currentLevel - 1),
                                .top = 11 * _textureTileSize,
                                .width = _textureTileSize,
                                .height = _textureTileSize};
            }
            rc->setTextureRect(previousRect);
        }
    }
}

void GameView::updatePlayerPositions() {
    for (const auto &workerData : _gameStateData.workers) {
        if (workerData.object == nullptr) {
            continue; // skip if worker is not yet placed on the board
        } else if (!workerData.isPlaced) {
            workerData.object->enabled = false; // disable worker if it is not placed on the board
            continue;
        }
        auto tile = _gameStateData.tileData[{workerData.position.first, workerData.position.second}]
                        .tileObject;
        if (tile == nullptr) {
            continue;
        }
        workerData.object->enabled = true; // enable player once it is placed on the board
        workerData.object->localTransform.position = {tile->localTransform.position.x,
                                                      tile->localTransform.position.y};
    }
}
} // namespace game