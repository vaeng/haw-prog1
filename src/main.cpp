#include "engine/Core.h"
#include "game/Scene.h"

auto startGame() -> void {
    auto core = engine::Core{{.title = "Sontorini Clone", .width = 672, .height = 768}};

    auto scene = game::createScene();
    core.loadScene(std::move(scene));

    core.start();

    core.run();
}

auto main() -> int {
    startGame();
    return 0;
}
