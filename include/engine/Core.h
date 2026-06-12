#pragma once

#include <SFML/Window/Event.hpp>

#include "Context.h"
#include "GameObject.h"

namespace engine {

class Core {
  public:
    void init(Context *context);

    void loadSceneTree(GameObject &&sceneRoot);

    void start();
    void handleEvents(float deltaTime);
    void update(float deltaTime);
    void render(float deltaTime);

    [[nodiscard]] auto getContext() const -> Context *;

    auto getRoot() -> GameObject &;

  private:
    GameObject root_{};
    Context *context_{};
};
} // namespace engine
