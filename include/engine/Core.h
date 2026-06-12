#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

#include "Context.h"
#include "GameObject.h"

namespace engine {

struct WindowConfig {
    std::string title{};
    unsigned int width{800};
    unsigned int height{600};
};

class Core {
  public:
    Core(WindowConfig config)
        : _window(sf::VideoMode({config.width, config.height}), config.title,
                  sf::Style::Titlebar | sf::Style::Close),
          _context{.window = &_window} {}
    void loadScene(GameObject &&sceneRoot);

    void start();
    void run();
    void handleEvents(float deltaTime);
    void update(float deltaTime);
    void render(float deltaTime);

    [[nodiscard]] auto getContext() const -> const Context &;

    auto getRoot() -> GameObject &;

  private:
    GameObject _root{};
    sf::RenderWindow _window;
    Context _context{};
};
} // namespace engine
