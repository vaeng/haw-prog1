#include <SFML/Graphics.hpp>

#include "engine/AnimationComponent.h"
#include "engine/Core.h"
#include "engine/MusicComponent.h"
#include "engine/RenderComponent.h"

auto createScene() -> engine::GameObject {
    engine::GameObject sceneRoot{};

    auto rick = std::make_unique<engine::GameObject>();

    auto texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile("assets/textures/rick.png")) {
        throw std::runtime_error("Failed to load texture");
    }

    auto renderComponent = rick->addComponent<engine::RenderComponent>(texture);
    auto frameInfo = engine::FrameInfo{.framesPerSecond = 20.F,
                                       .totalFrames = 43,
                                       .left = 0,
                                       .top = 0,
                                       .width = 498,
                                       .height = 374,
                                       .verticalOffset = 2,
                                       .horizontalOffset = 2,
                                       .horizontalFrameCount = 5,
                                       .verticalFrameCount = 9,
                                       .horizontalPadding = 4,
                                       .verticalPadding = 4};
    rick->addComponent<engine::AnimationComponent>(frameInfo);

    auto music = std::make_shared<sf::Music>();
    if (!music->openFromFile("assets/audio/Rick Astley - Never Gonna Give You Up.mp3")) {
        throw std::runtime_error("Failed to load music");
    }
    auto musicComponent = rick->addComponent<engine::MusicComponent>(music);
    musicComponent.play();
    sceneRoot.addChild(std::move(rick));

    return sceneRoot;
}

auto startGame() -> void {
    auto const config = engine::WindowConfig{
        .title = "Sontorini Clone",
        .width = 800,
        .height = 600,
    };
    auto core = engine::Core{config};
    auto scene = createScene();
    core.loadScene(std::move(scene));
    core.start();
    core.run();
}

auto main() -> int {
    startGame();
    return 0;
}
