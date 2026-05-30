#pragma once

#include "Context.h"
#include "GameObject.h"

class Game {
  public:
    Game(Context &context) : context_(context) {}

    void handleInput();
    void update(double deltaTime);
    void render(double deltaTime);

    auto getRoot() -> GameObject &;

  private:
    Context &context_;
    GameObject root_{};
};