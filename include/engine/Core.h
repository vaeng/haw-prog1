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

/// The main class of the engine, responsible for managing the game loop and rendering
///
/// Expected usage is to create an instance of Core, load a scene, and then call start() and run()
class Core {
  public:
    /// Initializes the engine with the given window configuration
    Core(WindowConfig config)
        : _window(sf::VideoMode({config.width, config.height}), config.title,
                  sf::Style::Titlebar | sf::Style::Close),
          _context{.window = &_window, .messageBus = &_messageBus} {}

    /// Loads a scene into the engine
    ///
    /// The sceneRoot will be moved and replace the current scene, if any
    void loadScene(GameObject &&sceneRoot);

    /// Starts the engine, initializing all components
    ///
    /// Components will be initialized in the order they were added to their respective GameObjects,
    /// and GameObjects will be initialized in a depth-first manner starting from the root, calling
    /// start() on each component. This should only be called once, and after loading the initial
    /// scene.
    void start();

    /// The main game loop, which will continue running until the window is closed
    ///
    /// This will call handleEvents(), update(), and render() in a loop
    void run();

    /// Processes all pending input and window events.
    ///
    /// Called once per frame. Dispatches to handleEvent() on all components.
    void handleEvents(float deltaTime);

    /// Updates the game state
    ///
    /// Called once per frame. Dispatches to update() on all components.
    void update(float deltaTime);

    /// Renders the current game state to the window
    ///
    /// Called once per frame. Dispatches to render(), sorting RenderComponents by layer and zIndex
    /// before rendering.
    void render(float deltaTime);

    /// Gets the current context
    ///
    /// Provides access to the SFML RenderWindow, in the future may also provide access to other
    /// global resources. The context is read-only
    [[nodiscard]] auto getContext() const -> const Context &;

    /// Gets the root GameObject of the current scene
    auto getRoot() -> GameObject &;

  private:
    GameObject _root{};
    sf::RenderWindow _window;
    Context _context{};
    MessageBus _messageBus{};
};
} // namespace engine
