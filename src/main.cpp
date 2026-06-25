#include "engine/Core.h"
#include "game/Scene.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

auto startGame() -> void {
    auto core = engine::Core{{.title = "Sontorini Clone", .width = 672, .height = 768}};
    sf::Image icon;
    auto success = icon.loadFromFile("assets/textures/Game-Icon.png");
    if (success) {
        core.getContext().window->setIcon(icon);
    }

    auto scene = game::createScene();
    core.loadScene(std::move(scene));

    core.start();

    core.run();
}

auto main() -> int {
    startGame();
    return 0;
}
