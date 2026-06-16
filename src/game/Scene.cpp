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

    auto board = std::make_unique<engine::GameObject>();
    auto gameManager = board->addComponent<game::GameManager>(2, 5);
    // center the board in the window
    board->localTransform.position = {.x = 400, .y = 300};
    board->localTransform.scale = {.x = 1.5f, .y = 1.5f};

    sceneRoot.addChild(std::move(board));
    return sceneRoot;
}
} // namespace game