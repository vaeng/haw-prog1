#include <SFML/Window/Event.hpp>

#include "engine/Component.h"
#include "engine/Core.h"
#include "engine/RenderComponent.h"

namespace engine {

auto Core::run() -> void {
    sf::Clock clock;
    while (_context.window->isOpen()) {
        auto deltaTime = clock.restart().asSeconds();
        handleEvents(deltaTime);
        update(deltaTime);
        render(deltaTime);
    }
}

void startGameObject(GameObject &gameObject) {
    gameObject.startComponents();
    for (auto &child : gameObject.getChildren()) {
        startGameObject(*child);
    }
}

void Core::start() { startGameObject(_root); }

void handleEventComponents(GameObject &gameObject, sf::Event event, float deltaTime) {
    if (!gameObject.enabled) {
        return;
    }

    // Update the current game object
    gameObject.handleEventComponents(event, deltaTime);

    // Recursively update children
    for (auto &child : gameObject.getChildren()) {
        handleEventComponents(*child, event, deltaTime);
    }
}

void Core::handleEvents(float deltaTime) {
    while (const std::optional event = _context.window->pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            _context.window->close();
        } else if (event == std::nullopt) {
            continue;
        }
        handleEventComponents(_root, event.value(), deltaTime);
    }
}

void updateGameObject(GameObject &gameObject, float deltaTime) {
    if (!gameObject.enabled) {
        return;
    }

    // Update the current game object
    gameObject.updateComponents(deltaTime);

    // Recursively update children
    for (auto &child : gameObject.getChildren()) {
        updateGameObject(*child, deltaTime);
    }
}

void Core::update(float deltaTime) { updateGameObject(_root, deltaTime); }
auto Core::getRoot() -> GameObject & { return _root; }

void renderGameObject(GameObject &gameObject, float deltaTime) {
    if (!gameObject.enabled) {
        return;
    }

    // Render the current game object
    auto *renderComponent = gameObject.getComponent<RenderComponent>();
    if (renderComponent != nullptr && renderComponent->enabled) {
        renderComponent->render(deltaTime);
    }

    // Recursively render children
    for (auto &child : gameObject.getChildren()) {
        renderGameObject(*child, deltaTime);
    }
}

auto Core::getContext() const -> const Context & { return _context; }

void Core::render(float deltaTime) {
    _context.window->clear();
    // Recursively render all game objects starting from the root
    renderGameObject(getRoot(), deltaTime);
    _context.window->display();
}

void Core::loadScene(GameObject &&sceneRoot) {
    _root = std::move(sceneRoot);
    _root.setCore(this);
}
} // namespace engine
