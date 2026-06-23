#include "game/GameView.h"
#include "engine/AnimationComponent.h"
#include "engine/GameObject.h"
#include "engine/RenderComponent.h"
#include "engine/Vector2.h"
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
    _buildingPlacedConnection =
        _messageBus->subscribe<BuildingPlacedMessage>([this](const BuildingPlacedMessage &message) {
            animatedBuildingPlacement(message.x, message.y, message.level);
        });
    _tileHoveredConnection = _messageBus->subscribe<TileHoveredMessage>(
        [this](const TileHoveredMessage &message) { tileHoveredPreview(message.x, message.y); });
    _workerPlacedConnection = _messageBus->subscribe<WorkerPlacedMessage>(
        [this](const WorkerPlacedMessage &message) { animatedWorkerPlacement(message.workerId); });
    _workerMovedConnection =
        _messageBus->subscribe<WorkerMovedMessage>([this](const WorkerMovedMessage &message) {
            workerMovementAnimation(message.workerId, message.originX, message.originY,
                                    message.destinationX, message.destinationY);
        });
    // trigger initial setup of labels and highlights based on initial game state
    fillBuildingTextureRects();

    // setup for arrow previews
    auto arrowParent = std::make_unique<engine::GameObject>();
    _arrowParent = arrowParent.get();
    owner->addChild(std::move(arrowParent));
    _moveArrowTexture =
        std::make_shared<engine::Texture>("assets/textures/Arrows-Move-Spritesheet.png");
    _buildArrowTexture =
        std::make_shared<engine::Texture>("assets/textures/Arrows-Build-Spritesheet.png");

    for (int i = 0; i < 8; ++i) {
        auto arrow = std::make_unique<engine::GameObject>();
        arrow->enabled = false;
        arrow->addComponent<engine::RenderComponent>(_moveArrowTexture, 100);
        auto arrowAnimationComponent = arrow->addComponent<engine::AnimationComponent>();
        arrowAnimationComponent->setFrameInfo({
            .framesPerSecond = 3,
            .totalFrames = 2,
            .width = _boardProperties.screenTileSize,
            .height = _boardProperties.screenTileSize,
            .verticalOffset = i * _boardProperties.screenTileSize,
            .horizontalOffset = 0,
            .horizontalFrameCount = 2,
            .verticalFrameCount = 1,
            .horizontalPadding = 0,
            .verticalPadding = 0,
        });
        arrowAnimationComponent->setLooping(true);
        arrowAnimationComponent->play();
        _arrowObjects[static_cast<Directions>(i)] = arrow.get();
        _arrowParent->addChild(std::move(arrow));
    }
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
            renderComponent->setTextureRect({.left = 0,
                                             .top = 0,
                                             .width = _boardProperties.screenTileSize,
                                             .height = _boardProperties.screenTileSize});
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
            building->addComponent<engine::AnimationComponent>();
            owner->addChild(std::move(building));
        }
    }
};

void GameView::createPlayers(engine::GameObject *owner) {
    auto player1Texture = std::make_shared<engine::Texture>(
        "assets/textures/Player-1-Spritesheet-Without-Towers.png");
    auto player2Texture = std::make_shared<engine::Texture>(
        "assets/textures/Player-2-Spritesheet-Without-Towers.png");
    auto playerSpriteHeight = (int)(_boardProperties.screenTileSize * 1.5f);
    for (const auto &[id, position, selected, placed, playerNumber] : _gameStateData.workers) {
        auto worker = std::make_unique<engine::GameObject>();
        worker->enabled = false; // disable worker until they are placed on the board
        worker->addComponent<engine::AnimationComponent>();
        if (playerNumber == 1) {
            auto workerRenderComponent =
                worker->addComponent<engine::RenderComponent>(player1Texture, 30);
            workerRenderComponent->setTextureRect({.left = 0,
                                                   .top = 0,
                                                   .width = _boardProperties.screenTileSize,
                                                   .height = playerSpriteHeight});
            workerRenderComponent->setPivot({.x = 0.5f, .y = 0.666f});

        } else {
            auto workerRenderComponent =
                worker->addComponent<engine::RenderComponent>(player2Texture, 30);
            workerRenderComponent->setTextureRect({.left = _boardProperties.screenTileSize,
                                                   .top = 0,
                                                   .width = _boardProperties.screenTileSize,
                                                   .height = playerSpriteHeight});
            workerRenderComponent->setPivot({.x = 0.5f, .y = 0.666f});
        }
        _workerObjects[id] = worker.get();
        owner->addChild(std::move(worker));
    }
    // two objects, that are exclusively used for showing the possible move preview when hovering
    // over tiles in the movement phase, so they need to be created here with the rest of the
    // workers, but will be enabled/positioned as needed in the tileHoveredPreview function
    auto player1MovePreview = std::make_unique<engine::GameObject>();
    player1MovePreview->enabled = false;
    auto player1MovePreviewRenderComponent =
        player1MovePreview->addComponent<engine::RenderComponent>(player1Texture, 50);
    _player1MovePreview = player1MovePreview.get();
    player1MovePreviewRenderComponent->setPivot({.x = 0.5f, .y = 0.666f});
    owner->addChild(std::move(player1MovePreview));

    auto player2MovePreview = std::make_unique<engine::GameObject>();
    player2MovePreview->enabled = false;
    auto player2MovePreviewRenderComponent =
        player2MovePreview->addComponent<engine::RenderComponent>(player2Texture, 50);
    _player2MovePreview = player2MovePreview.get();
    player2MovePreviewRenderComponent->setPivot({.x = 0.5f, .y = 0.666f});
    owner->addChild(std::move(player2MovePreview));

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
    auto labelHeight = _boardProperties.screenTileSize;
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
    auto winner1Rect =
        engine::Rect{.left = 0, .top = labelHeight * 6, .width = 154, .height = labelHeight};
    auto winner2Rect =
        engine::Rect{.left = 0, .top = labelHeight * 7, .width = 154, .height = labelHeight};
    auto restartRect =
        engine::Rect{.left = 0, .top = labelHeight * 8, .width = 177, .height = labelHeight};

    switch (_gameStateData.turn) {
    case Turn::Player1Placement:
    case Turn::Player1Movement:
    case Turn::Player1Build:
    case Turn::Player1Select:
        _activePlayerLabel->getComponent<engine::RenderComponent>()->setTextureRect(
            player1turnRect);
        break;
    case Turn::Player1Win:
        _activePlayerLabel->getComponent<engine::RenderComponent>()->setTextureRect(winner1Rect);
        break;
    case Turn::Player2Win:
        _activePlayerLabel->getComponent<engine::RenderComponent>()->setTextureRect(winner2Rect);
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
        _gameStateLabel->getComponent<engine::RenderComponent>()->setTextureRect(restartRect);
        break;
    case Turn::Player1Select:
    case Turn::Player2Select:
        _gameStateLabel->getComponent<engine::RenderComponent>()->setTextureRect(selectRect);
        break;
    }
}

void GameView::fillBuildingTextureRects() {
    auto buildingHeight = (int)(_boardProperties.screenTileSize * 1.25f);
    auto getBuildingTextureRect = [&](int row, int column) {
        return engine::Rect{.left = column * _boardProperties.screenTileSize,
                            .top = row * buildingHeight,
                            .width = _boardProperties.screenTileSize,
                            .height = buildingHeight};
    };
    _buildingTextureRects[BuildingLevel::None] =
        getBuildingTextureRect(0, 2); // use the an empty tile in the spritesheet for the None level
    _buildingTextureRects[BuildingLevel::Level1] = getBuildingTextureRect(4, 4);
    _buildingTextureRects[BuildingLevel::Level2] = getBuildingTextureRect(5, 3);
    _buildingTextureRects[BuildingLevel::Level3] = getBuildingTextureRect(6, 3);
    _buildingTextureRects[BuildingLevel::Dome] = getBuildingTextureRect(7, 7);
}

void GameView::highlightPossibleActions() {
    for (auto const &[id, position, selected, placed, number] : _gameStateData.workers) {
        if (selected) {
            highlightActionsWithArrows(position.first, position.second);
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
        auto ac = building->getComponent<engine::AnimationComponent>();
        if (ac != nullptr && ac->isPlaying()) {
            continue; // skip updating the texture rect if the building animation is still playing
        }
        if (rc != nullptr) {
            rc->setTextureRect(_buildingTextureRects[currentLevel]);
        }
    }
}

void GameView::updatePlayerPositions() {
    _player1MovePreview->enabled = false;
    _player2MovePreview->enabled = false;
    auto playerSpriteHeight = (int)(_boardProperties.screenTileSize * 1.5f);
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

        // set sprite based on building level
        auto workerRenderComponent = _workerObjects[id]->getComponent<engine::RenderComponent>();
        if (workerRenderComponent == nullptr) {
            continue;
        }
        auto ac = _workerObjects[id]->getComponent<engine::AnimationComponent>();
        if (ac != nullptr && ac->isPlaying()) {
            continue; // skip updating the texture rect if the worker animation is still playing
        }
        _workerObjects[id]->localTransform.position = {tile->localTransform.position.x,
                                                       tile->localTransform.position.y};
        auto buildingLevel = _gameStateData.tileData[position].buildingLevel;
        workerRenderComponent->setTextureRect({.left = 0,
                                               .top = playerSpriteHeight * (int)buildingLevel,
                                               .width = _boardProperties.screenTileSize,
                                               .height = playerSpriteHeight});
    }
}

void GameView::animatedBuildingPlacement(int x, int y, BuildingLevel level) {
    auto building = _buildings[{x, y}];
    if (building == nullptr || level == BuildingLevel::None) {
        return;
    }
    auto animationComponent = building->getComponent<engine::AnimationComponent>();
    if (animationComponent == nullptr) {
        return;
    }
    auto buildingHeight = (int)(_boardProperties.screenTileSize * 1.25f);
    auto getBuildingFrameInfo = [&](int row, int column, int totalFrames) {
        return engine::FrameInfo{
            .framesPerSecond = 8,
            .totalFrames = totalFrames,
            .width = _boardProperties.screenTileSize,
            .height = buildingHeight,
            .verticalOffset = row * buildingHeight,
            .horizontalOffset = column * _boardProperties.screenTileSize,
            .horizontalFrameCount = totalFrames,
            .verticalFrameCount = 1,
            .horizontalPadding = 0,
            .verticalPadding = 0,
        };
    };
    auto frameInfo = engine::FrameInfo{};

    switch (level) {
    case BuildingLevel::None:
        frameInfo = getBuildingFrameInfo(
            0, 1, 1); // use the an empty tile in the spritesheet for the None level
        break;
    case BuildingLevel::Level1:
        frameInfo = getBuildingFrameInfo(4, 0, 5);
        break;
    case BuildingLevel::Level2:
        frameInfo = getBuildingFrameInfo(5, 0, 4);
        break;
    case BuildingLevel::Level3:
        frameInfo = getBuildingFrameInfo(6, 0, 4);
        break;
    case BuildingLevel::Dome:
        frameInfo = getBuildingFrameInfo(7, 0, 8);
        break;
    }
    animationComponent->setFrameInfo(frameInfo);
    animationComponent->play();
}

void GameView::animatedWorkerPlacement(int workerId) {
    auto playerSpriteHeight = (int)(_boardProperties.screenTileSize * 1.5f);
    auto worker = _workerObjects[workerId];
    if (worker == nullptr) {
        return;
    }
    auto animationComponent = worker->getComponent<engine::AnimationComponent>();
    if (animationComponent == nullptr) {
        return;
    }
    auto buildingLevel =
        (int)_gameStateData.tileData[_gameStateData.workers[workerId].position].buildingLevel;
    animationComponent->setFrameInfo({
        .framesPerSecond = 10,
        .totalFrames = 7,
        .width = _boardProperties.screenTileSize,
        .height = playerSpriteHeight,
        .verticalOffset = (12 + buildingLevel) * playerSpriteHeight,
        .horizontalOffset = 0 * _boardProperties.screenTileSize,
        .horizontalFrameCount = 7,
        .verticalFrameCount = 1,
        .horizontalPadding = 0,
        .verticalPadding = 0,
    });
    // move to the worker's position so the animation plays in the correct location, as the normal
    // update skips animated workers
    auto tile = _tileObjects[{_gameStateData.workers[workerId].position.first,
                              _gameStateData.workers[workerId].position.second}];
    if (tile != nullptr) {
        worker->localTransform.position = {tile->localTransform.position.x,
                                           tile->localTransform.position.y};
    }
    animationComponent->play();
}

void GameView::workerMovementAnimation(int workerId, int originX, int originY, int destinationX,
                                       int destinationY) {
    auto playerSpriteHeight = (int)(_boardProperties.screenTileSize * 1.5f);
    auto worker = _workerObjects[workerId];
    if (worker == nullptr) {
        return;
    }
    auto animationComponent = worker->getComponent<engine::AnimationComponent>();
    if (animationComponent == nullptr) {
        return;
    }
    auto newBuildingLevel =
        (int)_gameStateData.tileData[{destinationX, destinationY}].buildingLevel;
    auto appearFrameInfo = engine::FrameInfo{
        .framesPerSecond = 10,
        .totalFrames = 7,
        .width = _boardProperties.screenTileSize,
        .height = playerSpriteHeight,
        .verticalOffset = (12 + newBuildingLevel) * playerSpriteHeight,
        .horizontalOffset = 0 * _boardProperties.screenTileSize,
        .horizontalFrameCount = 7,
        .verticalFrameCount = 1,
        .horizontalPadding = 0,
        .verticalPadding = 0,
    };

    // move to the worker's position so the animation plays in the correct location, as the normal
    // update skips animated workers
    auto tile = _tileObjects[{destinationX, destinationY}];
    if (tile != nullptr) {
        worker->localTransform.position = {tile->localTransform.position.x,
                                           tile->localTransform.position.y};
    }
    auto oldBuildingLevel = (int)_gameStateData.tileData[{originX, originY}].buildingLevel;
    auto vectorToMove = _tileObjects[{originX, originY}]->localTransform.position -
                        _tileObjects[{destinationX, destinationY}]->localTransform.position;
    auto disappearFrameInfo =
        engine::FrameInfo{.framesPerSecond = 10,
                          .totalFrames = 7,
                          .width = _boardProperties.screenTileSize,
                          .height = playerSpriteHeight,
                          .verticalOffset = (8 + oldBuildingLevel) * playerSpriteHeight,
                          .horizontalOffset = 0 * _boardProperties.screenTileSize,
                          .horizontalFrameCount = 7,
                          .verticalFrameCount = 1,
                          .horizontalPadding = 0,
                          .verticalPadding = 0,
                          .positionDelta = vectorToMove};
    animationComponent->setFrameInfo(disappearFrameInfo);
    animationComponent->queueAnimation(appearFrameInfo);
    animationComponent->play();
}

void GameView::tileHoveredPreview(int x, int y) {
    auto building = _buildings[{x, y}];
    if (building == nullptr) {
        return;
    }
    auto const &tileData = _gameStateData.tileData[{x, y}];
    auto highlightType = tileData.highlight;
    auto buildingLevel = (int)tileData.buildingLevel;
    auto renderComponent = building->getComponent<engine::RenderComponent>();
    if (renderComponent == nullptr) {
        return;
    }
    auto buildingHeight = (int)(_boardProperties.screenTileSize * 1.25f);
    auto playerSpriteHeight = (int)(_boardProperties.screenTileSize * 1.5f);
    auto getBuildingPreviewTexstureRect = [&](int row, int column) {
        return engine::Rect{.left = column * _boardProperties.screenTileSize,
                            .top = row * buildingHeight,
                            .width = _boardProperties.screenTileSize,
                            .height = buildingHeight};
    };
    auto getPlayerPreviewTextureRect = [&](int row, int column) {
        return engine::Rect{.left = column * _boardProperties.screenTileSize,
                            .top = playerSpriteHeight * row,
                            .width = _boardProperties.screenTileSize,
                            .height = playerSpriteHeight};
    };

    switch (_gameStateData.turn) {
    case Turn::Player1Build:
    case Turn::Player2Build:
        updateBoard(); // reset any previous highlights or previews
        if (highlightType == HighlightType::CanBuild) {
            renderComponent->setTextureRect(
                getBuildingPreviewTexstureRect(buildingLevel, 0)); // preview the level 1 building
        }
        break;
    case Turn::Player1Movement:
    case Turn::Player2Movement:
        updatePlayerPositions(); // reset any previous highlights or previews
        if (highlightType == HighlightType::CanMove) {
            if (_gameStateData.turn == Turn::Player1Movement) {
                renderComponent = _player1MovePreview->getComponent<engine::RenderComponent>();
                _player1MovePreview->localTransform.position = building->localTransform.position;
                _player1MovePreview->enabled = true;
            } else {
                renderComponent = _player2MovePreview->getComponent<engine::RenderComponent>();
                _player2MovePreview->localTransform.position = building->localTransform.position;
                _player2MovePreview->enabled = true;
            }
            renderComponent->setTextureRect(getPlayerPreviewTextureRect(
                4 + buildingLevel, 0)); // preview the highlighted tile for movement
        }
        break;
    default:
        break;
    }
}

std::pair<int, int> GameView::getTileFromDirection(int x, int y, Directions direction) {
    switch (direction) {
    case Directions::Up:
        return {x, y - 1};
    case Directions::Down:
        return {x, y + 1};
    case Directions::Left:
        return {x - 1, y};
    case Directions::Right:
        return {x + 1, y};
    case Directions::UpLeft:
        return {x - 1, y - 1};
    case Directions::UpRight:
        return {x + 1, y - 1};
    case Directions::DownLeft:
        return {x - 1, y + 1};
    case Directions::DownRight:
        return {x + 1, y + 1};
    }
}

void GameView::highlightActionsWithArrows(int x, int y) {
    _arrowParent->localTransform.position = _tileObjects[{x, y}]->localTransform.position;
    for (auto &[direction, arrow] : _arrowObjects) {
        auto [targetX, targetY] = getTileFromDirection(x, y, direction);
        if (!_gameStateData.tileData.contains({targetX, targetY})) {
            arrow->enabled = false; // out of bounds
            continue;
        }
        auto highlightType = _gameStateData.tileData[{targetX, targetY}].highlight;
        auto arrowRenderComponent = arrow->getComponent<engine::RenderComponent>();
        if (arrowRenderComponent == nullptr) {
            continue;
        }
        auto arrowAnimationComponent = arrow->getComponent<engine::AnimationComponent>();
        if (arrowAnimationComponent == nullptr) {
            continue;
        }
        if (highlightType == HighlightType::CanMove) {
            arrowRenderComponent->setTexture(_moveArrowTexture);
            arrowAnimationComponent->play();
            arrow->enabled = true;
        } else if (highlightType == HighlightType::CanBuild) {
            arrowRenderComponent->setTexture(_buildArrowTexture);
            arrowAnimationComponent->play();
            arrow->enabled = true;
        } else {
            arrowAnimationComponent->stop();
            arrow->enabled = false;
        }
    }
}

} // namespace game