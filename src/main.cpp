#include <SFML/Graphics.hpp>

#include "game/AnimationComponent.h"
#include "game/Game.h"
#include "game/MusicComponent.h"
#include "game/RenderComponent.h"

auto createScene() -> GameObject {
    GameObject sceneRoot{};

    auto rick = std::make_unique<GameObject>();

    auto texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile("assets/textures/rick.png")) {
        throw std::runtime_error("Failed to load texture");
    }

    auto renderComponent = rick->addComponent<RenderComponent>(texture);
    auto frameInfo = FrameInfo{.framesPerSecond = 20.F,
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
    rick->addComponent<AnimationComponent>(frameInfo);

    auto music = std::make_shared<sf::Music>();
    if (!music->openFromFile("assets/audio/Rick Astley - Never Gonna Give You Up.mp3")) {
        throw std::runtime_error("Failed to load music");
    }
    auto musicComponent = rick->addComponent<MusicComponent>(music);
    musicComponent.play();
    sceneRoot.addChild(std::move(rick));

    return sceneRoot;
}

auto startGame() -> void {
    const auto *TITLE = "Sontorini Clone";
    const auto HORIZONTAL_RESOLUTION = 800U;
    const auto VERTICAL_RESOLUTION = 600U;
    sf::RenderWindow window(sf::VideoMode({HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION}), TITLE,
                            sf::Style::Titlebar | sf::Style::Close);

    auto game = Game{};
    Context context{.window = &window};
    game.init(&context);
    auto scene = createScene();
    game.loadSceneTree(std::move(scene));
    game.start();
    sf::Clock clock;
    while (window.isOpen()) {
        auto deltaTime = clock.restart().asSeconds();
        game.handleEvents(deltaTime);
        game.update(deltaTime);
        game.render(deltaTime);
    }
}

auto main() -> int {
    startGame();
    return 0;
}
