#include "game/Scene.h"

#include "engine/AnimationComponent.h"
#include "engine/GameObject.h"
#include "engine/MusicComponent.h"
#include "engine/RenderComponent.h"
#include "engine/Texture.h"
#include "engine/Vector2.h"

#include "game/GameManager.h"

namespace game {

auto exampleWithRick() -> engine::GameObject {
    engine::GameObject sceneRoot{};

    auto rick = std::make_unique<engine::GameObject>();
    rick->localTransform.position = {.x = 400, .y = 300};

    auto texture = std::make_shared<engine::Texture>("assets/textures/rick.png");

    auto renderComponent = rick->addComponent<engine::RenderComponent>(texture);
    auto frameInfo = engine::FrameInfo{.framesPerSecond = 20.F,
                                       .totalFrames = 43,
                                       .width = 498,
                                       .height = 374,
                                       .verticalOffset = 2,
                                       .horizontalOffset = 2,
                                       .horizontalFrameCount = 5,
                                       .verticalFrameCount = 9,
                                       .horizontalPadding = 4,
                                       .verticalPadding = 4};
    rick->addComponent<engine::AnimationComponent>(frameInfo);

    auto music =
        std::make_shared<engine::Music>("assets/audio/Rick Astley - Never Gonna Give You Up.mp3");
    auto musicComponent = rick->addComponent<engine::MusicComponent>(music);
    musicComponent->play();
    sceneRoot.addChild(std::move(rick));

    return sceneRoot;
}

auto createScene() -> engine::GameObject {
    engine::GameObject sceneRoot{};
    auto tiles = std::make_shared<engine::Texture>("assets/textures/tiles.png");

    auto board = std::make_unique<engine::GameObject>();
    board->addComponent<game::GameManager>(5, 32);
    // center the board in the window
    board->localTransform.position = {.x = 400, .y = 300};

    // textureRects cannot be repeated in SFML, so we add children
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            auto tile = std::make_unique<engine::GameObject>();
            tile->localTransform.position = {.x = i * 32.f, .y = j * 32.f};
            auto renderComponent = tile->addComponent<engine::RenderComponent>(tiles);
            renderComponent->setTextureRect({.left = 0, .top = 0, .width = 32, .height = 32});
            board->addChild(std::move(tile));
        }
    }

    sceneRoot.addChild(std::move(board));
    return sceneRoot;
}
} // namespace game