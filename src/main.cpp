#include "engine/Core.h"
#include "game/Scene.h"

auto startGame() -> void {
    auto const config = engine::WindowConfig{
        .title = "Sontorini Clone",
        .width = 800,
        .height = 600,
    };
    auto core = engine::Core{config};
    auto scene = game::createScene();
    core.loadScene(std::move(scene));
    core.start();
    core.run();
}

auto main() -> int {
    startGame();
    return 0;
}
