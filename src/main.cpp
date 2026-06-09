#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/WindowEnums.hpp>

#include "game/AnimationComponent.h"
#include "game/Game.h"
#include "game/RenderComponent.h"

auto createScene() -> GameObject {
    GameObject sceneRoot{};

    auto rick = std::make_unique<GameObject>();

    auto texture = std::make_shared<sf::Texture>();
    if (!texture->loadFromFile("assets/textures/rick.png")) {
        throw std::runtime_error("Failed to load texture");
    }

    auto renderComponent = rick->addComponent<RenderComponent>(texture);

    rick->addComponent<AnimationComponent>();

    sceneRoot.addChild(std::move(rick));

    return sceneRoot;
}

auto startGame() -> void {
    const auto *TITLE = "Sontorini Clone";
    const auto HORIZONTAL_RESOLUTION = 800U;
    const auto VERTICAL_RESOLUTION = 600U;
    sf::RenderWindow window(sf::VideoMode({HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION}), TITLE,
                            sf::Style::Titlebar | sf::Style::Close);

    auto &game = Game::instance();
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
