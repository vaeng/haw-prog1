#pragma once

#include "Context.h"
#include "GameObject.h"
#include <SFML/Window/Event.hpp>

class Game {
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