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
    fillBuildingTextureRects();
}

void GameView::createBoard(engine::GameObject *owner) {

    // the background texture is placed above the tiles, and has a cutout, that is off center, so we
    // need to offset the positions to match the centered tiles
    auto backgroundTexture = std::make_shared<engine::Texture>("assets/textures/Board.png");
    auto background = std::make_unique<engine::GameObject>();
    auto backgroundRenderComponent =
        background->addComponent<engine::RenderComponent>(backgroundTexture, -10);
    owner->addChild(std::move(background));

    float spacing = _boardProperties.screenTileSize;
    auto tiles = std::make_shared<engine::Texture>("assets/textures/Tile.png");
    auto buildingTiles = std::make_shared<engine::Texture>("assets/textures/Tower-Spritesheet.png");

    auto tilesPerSide = _boardProperties.numTiles /
                        2; // number of tiles from center to edge, e.g. 2 for a 5x5 board

    for (int i = -tilesPerSide; i <= tilesPerSide; ++i) {
        for (int j = -tilesPerSide; j <= tilesPerSide; ++j) {
            // ground tile
            auto tile = std::make_unique<engine::GameObject>();
            _tileObjects[{i, j}] = tile.get();
            tile->localTransform.position = {
                .x = i * spacing, .y = j * spacing + _boardProperties.verticalBoardOffset};
            auto renderComponent = tile->addComponent<engine::RenderComponent>(tiles);
            renderComponent->setTextureRect(
                {.left = 0, .top = 0, .width = _textureTileSize, .height = _textureTileSize});
            owner->addChild(std::move(tile));

            // building tile
            auto building = std::make_unique<engine::GameObject>();
            _buildings[{i, j}] = building.get();
            building->localTransform.position = {
                .x = i * spacing, .y = j * spacing + _boardProperties.verticalBoardOffset};
            auto buildingRC = building->addComponent<engine::RenderComponent>(buildingTiles, 10);
            buildingRC->setTextureRect(_buildingTextureRects[BuildingLevel::None]);
            buildingRC->setPivot(
                {.x = 0.5f, .y = 0.6f}); // adjust for building height and center on tile

            owner->addChild(std::move(building));
        }
    }
};

void GameView::createPlayers(engine::GameObject *owner) {
    auto player1Texture = std::make_shared<engine::Texture>(
        "assets/textures/Player-1-Spritesheet-Without-Towers.png");
    auto player2Texture = std::make_shared<engine::Texture>(
        "assets/textures/Player-2-Spritesheet-Without-Towers.png");

    for (const auto &[id, position, selected, placed, playerNumber] : _gameStateData.workers) {
        auto worker = std::make_unique<engine::GameObject>();
        worker->enabled = false; // disable worker until they are placed on the board
        if (playerNumber == 1) {
            auto workerRenderComponent =
                worker->addComponent<engine::RenderComponent>(player1Texture, 30);
            workerRenderComponent->setTextureRect(
                {.left = 0, .top = 0, .width = _textureTileSize, .height = _playerSpriteHeight});
            workerRenderComponent->setPivot({.x = 0.5f, .y = 0.666f});
        } else {
            auto workerRenderComponent =
                worker->addComponent<engine::RenderComponent>(player2Texture, 30);
            workerRenderComponent->setTextureRect({.left = _textureTileSize,
                                                   .top = 0,
                                                   .width = _textureTileSize,
                                                   .height = _playerSpriteHeight});
            workerRenderComponent->setPivot({.x = 0.5f, .y = 0.666f});
        }
        _workerObjects[id] = worker.get();
        owner->addChild(std::move(worker));
    }

    _gameStateData.workers[0].isSelected = true; // default to player 1 selected at start of game
}

void GameView::setupLabels(engine::GameObject *owner) {
    auto textTexture = std::make_shared<engine::Texture>("assets/textures/Labels-sheet.png");
    auto activePlayerLabel = std::make_unique<engine::GameObject>();
    activePlayerLabel->localTransform.position = {0, -(float)(_boardProperties.numTiles / 2 + 1) -
                                                         _boardProperties.screenTileSize * 3.1f};
    auto activePlayerLabelRenderComponent =
        activePlayerLabel->addComponent<engine::RenderComponent>(textTexture, 10);
    _activePlayerLabel = activePlayerLabel.get();
    owner->addChild(std::move(activePlayerLabel));

    auto gameStateLabel = std::make_unique<engine::GameObject>();
    gameStateLabel->localTransform.position = {0, -(float)(_boardProperties.numTiles / 2 + 1) -
                                                      _boardProperties.screenTileSize * 2.5f};
    auto gameStateLabelRenderComponent =
        gameStateLabel->addComponent<engine::RenderComponent>(textTexture, 10);
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
    auto labelHeight = 32;
    auto player1turnRect = engine::Rect{.left = 0, .top = 0, .width = 92, .height = labelHeight};
    auto player2turnRect =
        engine::Rect{.left = 0, .top = labelHeight * 1, .width = 93, .height = labelHeight};
    auto placeRect =
        engine::Rect{.left = 0, .top = labelHeight * 2, .width = 132, .height = labelHeight};
    auto moveRect =
        engine::Rect{.left = 0, .top = labelHeight * 3, .width = 73, .height = labelHeight};
    auto buildRect =
        engine::Rect{.left = 0, .top = labelHeight * 4, .width = 73, .height = labelHeight};
    auto selectRect =
        engine::Rect{.left = 0, .top = labelHeight * 5, .width = 146, .height = labelHeight};
    auto winnerRect =
        engine::Rect{.left = 0, .top = labelHeight * 5, .width = 300, .height = labelHeight};
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

void GameView::fillBuildingTextureRects() {
    auto buildingHeight = (int)(_textureTileSize * 1.25f);
    auto getBuildingTextureRect = [&](int row, int column) {
        return engine::Rect{.left = column * _textureTileSize,
                            .top = row * buildingHeight,
                            .width = _textureTileSize,
                            .height = buildingHeight};
    };
    _buildingTextureRects[BuildingLevel::None] =
        getBuildingTextureRect(0, 1); // use the an empty tile in the spritesheet for the None level
    _buildingTextureRects[BuildingLevel::Level1] = getBuildingTextureRect(4, 4);
    _buildingTextureRects[BuildingLevel::Level2] = getBuildingTextureRect(5, 3);
    _buildingTextureRects[BuildingLevel::Level3] = getBuildingTextureRect(6, 3);
    _buildingTextureRects[BuildingLevel::Dome] = getBuildingTextureRect(7, 7);
}

void GameView::highlightPossibleActions() {
    for (auto &[position, data] : _gameStateData.tileData) {
        switch (data.highlight) {
        case HighlightType::CanMove:
            _tileObjects[position]->getComponent<engine::RenderComponent>()->setTint(
                {150, 150, 255});
            break;
        case HighlightType::CanBuild:
            _tileObjects[position]->getComponent<engine::RenderComponent>()->setTint(
                {150, 255, 150});
            break;
        case HighlightType::BlockedMove:
        case HighlightType::BlockedBuild:
            _tileObjects[position]->getComponent<engine::RenderComponent>()->setTint(
                {255, 150, 150});
            break;
        case HighlightType::None:
            _tileObjects[position]->getComponent<engine::RenderComponent>()->setTint(
                {255, 255, 255});
            break;
        }
    }
}

void GameView::updateBoard() {
    // Reset tile data and visuals
    for (auto &[tilePos, data] : _gameStateData.tileData) {
        auto currentLevel = data.buildingLevel;
        auto building = _buildings[tilePos];
        if (building == nullptr) {
            continue;
        }
        auto rc = building->getComponent<engine::RenderComponent>();
        if (rc != nullptr) {
            rc->setTextureRect(_buildingTextureRects[currentLevel]);
        }
    }
}

void GameView::updatePlayerPositions() {
    for (const auto &[id, position, selected, placed, playerNumber] : _gameStateData.workers) {
        if (_workerObjects[id] == nullptr) {
            continue; // skip if worker is not yet placed on the board
        } else if (!placed) {
            _workerObjects[id]->enabled = false; // disable worker if it is not placed on the board
            continue;
        }
        auto tile = _tileObjects[{position.first, position.second}];
        if (tile == nullptr) {
            continue;
        }
        _workerObjects[id]->enabled = true; // enable worker once it is placed on the board
        _workerObjects[id]->localTransform.position = {tile->localTransform.position.x,
                                                       tile->localTransform.position.y};
        // set sprite based on building level
        auto workerRenderComponent = _workerObjects[id]->getComponent<engine::RenderComponent>();
        if (workerRenderComponent == nullptr) {
            continue;
        }
        auto buildingLevel = _gameStateData.tileData[position].buildingLevel;
        workerRenderComponent->setTextureRect({.left = 0,
                                               .top = _playerSpriteHeight * (int)buildingLevel,
                                               .width = _textureTileSize,
                                               .height = _playerSpriteHeight});
    }
}
} // namespace game