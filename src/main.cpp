#include "engine/Core.h"
#include "game/Scene.h"

auto startGame() -> void {
    auto core = engine::Core{{.title = "Sontorini Clone", .width = 800, .height = 600}};

    auto scene = game::createScene();
    core.loadScene(std::move(scene));

    core.start();

    core.run();
}

auto main() -> int {
    startGame();
    return 0;
}
