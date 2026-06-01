#pragma once

#include "Context.h"
#include "GameObject.h"

class Game {
  public:
    static auto instance() -> Game & {
        if (instance_ == nullptr) {
            instance_ = new Game();
        }
        return *instance_;
    }
    static void init();

    static void handleInput();
    static void update(float deltaTime);
    static void render(float deltaTime);

    static auto getRoot() -> GameObject &;

  private:
    Game() = default;
    static Game *instance_;
    Context &context_{};
    GameObject root_{};
};