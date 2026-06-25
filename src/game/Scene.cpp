#include "game/Scene.h"

#include "engine/AnimationComponent.h"
#include "engine/Core.h"
#include "engine/GameObject.h"
#include "engine/MusicComponent.h"
#include "engine/RenderComponent.h"
#include "engine/Texture.h"
#include "engine/Vector2.h"

#include "engine/RenderComponent.h"
#include "game/ButtonComponent.h"
#include "game/GameManager.h"
#include "game/GameState.h"

#include <memory>

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

auto gameScene() -> engine::GameObject {
    engine::GameObject sceneRoot{};

    return sceneRoot;
}
auto mainScene() -> engine::GameObject {

    engine::GameObject sceneRoot{};

    auto normalSize = engine::Vector2{.x = 0.8, .y = 0.8};
    auto hoverSize = engine::Vector2{.x = 0.9, .y = 0.9};

    auto board = std::make_unique<engine::GameObject>();
    board->enabled = false; // initially disabled, will be enabled when the game starts
    auto gameManager = board->addComponent<game::GameManager>(2, 5);

    board->localTransform.position = {.x = 224 * 1.5f, .y = 256 * 1.5f};
    board->localTransform.scale = {.x = 3.0f, .y = 3.0f};

    auto background = std::make_unique<engine::GameObject>();
    auto rc = background->addComponent<engine::RenderComponent>(
        std::make_shared<engine::Texture>("assets/textures/Home-Screen.png"));
    rc->setTextureRect({0, 0, 224, 256});
    background->localTransform.position = {.x = 224 * 1.5f, .y = 256 * 1.5f};
    background->localTransform.scale = {.x = 3.0f, .y = 3.0f};

    // buttons
    auto buttonLabels = std::make_shared<engine::Texture>("assets/textures/Menu-Buttons.png");
    // start button
    auto startButton = std::make_unique<engine::GameObject>();
    startButton->localTransform.position = {.x = 0, .y = -35};
    startButton->localTransform.scale = normalSize;
    auto startRC = startButton->addComponent<engine::RenderComponent>(buttonLabels, 10);
    startRC->setTextureRect({.left = 0, .top = 0, .width = 96, .height = 48});
    auto startButtonComponent = startButton->addComponent<game::ButtonComponent>();
    startButtonComponent->setButtonBounds({.left = 0, .top = 0, .width = 96, .height = 26});
    startButtonComponent->setOnReleaseCallback(
        [boardPtr = board.get(), backgroundPtr = background.get(), startRC]() {
            boardPtr->enabled = true;
            backgroundPtr->enabled = false;
        });
    startButtonComponent->setOnHoverCallback(
        [ptr = startButton.get(), hoverSize, normalSize, startRC]() {
            ptr->localTransform.scale = hoverSize;
            startRC->setTextureRect({.left = 96, .top = 0, .width = 96, .height = 48});
        });
    startButtonComponent->setOnMouseExitCallback(
        [ptr = startButton.get(), hoverSize, normalSize, startRC]() {
            ptr->localTransform.scale = normalSize;
            startRC->setTextureRect({.left = 0, .top = 0, .width = 96, .height = 48});
        });
    startButtonComponent->setOnClickCallback([startRC]() {
        startRC->setTextureRect({.left = 96 * 2, .top = 0, .width = 96, .height = 48});
    });

    auto howToScreen = std::make_unique<engine::GameObject>();
    howToScreen->enabled = false;
    auto howToText = std::make_shared<engine::Texture>("assets/textures/HowTo.png");
    auto howToScreenRC = howToScreen->addComponent<engine::RenderComponent>(howToText, 30);
    howToScreenRC->setTextureRect({.left = 0, .top = 0, .width = 224, .height = 256});

    auto howToButton = std::make_unique<engine::GameObject>();
    howToButton->localTransform.scale = normalSize;
    howToButton->localTransform.position = {.x = 0, .y = 0};
    auto howToRC = howToButton->addComponent<engine::RenderComponent>(buttonLabels, 10);
    howToRC->setTextureRect({.left = 0, .top = 48, .width = 96, .height = 48});
    auto howToButtonComponent = howToButton->addComponent<game::ButtonComponent>();
    howToButtonComponent->setButtonBounds({.left = 0, .top = 0, .width = 96, .height = 26});
    auto quitButton = std::make_unique<engine::GameObject>();
    quitButton->localTransform.position = {.x = 0, .y = 35};
    quitButton->localTransform.scale = normalSize;
    auto quitRC = quitButton->addComponent<engine::RenderComponent>(buttonLabels, 10);
    quitRC->setTextureRect({.left = 0, .top = 2 * 48, .width = 96, .height = 48});
    auto quitButtonComponent = quitButton->addComponent<game::ButtonComponent>();
    quitButtonComponent->setButtonBounds({.left = 0, .top = 0, .width = 96, .height = 26});
    quitButtonComponent->setOnReleaseCallback([quitButtonPtr = quitButton.get()]() {
        quitButtonPtr->getCore()->getContext().window->close();
    });
    quitButtonComponent->setOnHoverCallback(
        [ptr = quitButton.get(), hoverSize, normalSize, quitRC]() {
            ptr->localTransform.scale = hoverSize;
            quitRC->setTextureRect({.left = 96, .top = 2 * 48, .width = 96, .height = 48});
        });
    quitButtonComponent->setOnMouseExitCallback(
        [ptr = quitButton.get(), hoverSize, normalSize, quitRC]() {
            ptr->localTransform.scale = normalSize;
            quitRC->setTextureRect({.left = 0, .top = 2 * 48, .width = 96, .height = 48});
        });
    quitButtonComponent->setOnClickCallback([quitRC]() {
        quitRC->setTextureRect({.left = 96 * 2, .top = 2 * 48, .width = 96, .height = 48});
    });

    auto closeHowToButton = std::make_unique<engine::GameObject>();
    closeHowToButton->localTransform.position = {.x = 224 / 2 - 3, .y = -256 / 2 + 4};
    closeHowToButton->enabled = false;
    auto closeHowToRC = closeHowToButton->addComponent<engine::RenderComponent>(buttonLabels, 40);
    closeHowToRC->setTextureRect({.left = 0, .top = 33, .width = 6, .height = 8});
    auto closeHowToButtonComponent = closeHowToButton->addComponent<game::ButtonComponent>();
    closeHowToButtonComponent->setOnReleaseCallback(
        [howToScreenPtr = howToScreen.get(), backgroundPtr = background.get(),
         startButtonPtr = startButton.get(), howToButtonPtr = howToButton.get(),
         howToClosePtr = closeHowToButton.get(), quitButtonPtr = quitButton.get()]() {
            howToScreenPtr->enabled = false;
            howToClosePtr->enabled = false;
            // reactivate other buttons
            startButtonPtr->enabled = true;
            howToButtonPtr->enabled = true;
            quitButtonPtr->enabled = true;
        });
    howToButtonComponent->setOnReleaseCallback(
        [howToScreenPtr = howToScreen.get(), backgroundPtr = background.get(),
         startButtonPtr = startButton.get(), howToButtonPtr = howToButton.get(),
         quitButtonPtr = quitButton.get(), howToClosePtr = closeHowToButton.get()]() {
            howToScreenPtr->enabled = true;
            howToClosePtr->enabled = true;
            // deactivate other buttons
            quitButtonPtr->enabled = false;
            startButtonPtr->enabled = false;
            howToButtonPtr->enabled = false;
        });
    howToButtonComponent->setOnHoverCallback([ptr = howToButton.get(), hoverSize, howToRC]() {
        ptr->localTransform.scale = hoverSize;
        howToRC->setTextureRect({.left = 96, .top = 48, .width = 96, .height = 48});
    });
    howToButtonComponent->setOnMouseExitCallback([ptr = howToButton.get(), normalSize, howToRC]() {
        ptr->localTransform.scale = normalSize;
        howToRC->setTextureRect({.left = 0, .top = 48, .width = 96, .height = 48});
    });
    howToButtonComponent->setOnClickCallback([howToRC]() {
        howToRC->setTextureRect({.left = 96 * 2, .top = 48, .width = 96, .height = 48});
    });

    background->addChild(std::move(startButton));
    background->addChild(std::move(howToButton));
    background->addChild(std::move(howToScreen));
    background->addChild(std::move(quitButton));
    background->addChild(std::move(closeHowToButton));

    sceneRoot.addChild(std::move(background));
    sceneRoot.addChild(std::move(board));

    return sceneRoot;
}
auto createScene() -> engine::GameObject { return mainScene(); }
} // namespace game