#include "game/GameView.h"
#include "engine/GameObject.h"
#include "engine/RenderComponent.h"
#include "game/GameManager.h"

namespace game {

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
            return;
        }
        auto rc = tile->getComponent<engine::RenderComponent>();
        if (rc != nullptr) {
            auto previousRect = rc->getTextureRect();
            if (currentLevel == BuildingLevel::None) {
                previousRect = {
                    .left = 0, .top = 0, .width = _textureTileSize, .height = _textureTileSize};
            } else {
                previousRect = {.left = _textureTileSize * (int)currentLevel - 1,
                                .top = 11 * _textureTileSize,
                                .width = _textureTileSize,
                                .height = _textureTileSize};
            }
            rc->setTextureRect(previousRect);
        }
    }
}

} // namespace game