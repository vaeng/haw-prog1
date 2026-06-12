#include <SFML/Window/Event.hpp>

#include "engine/Component.h"
#include "engine/Core.h"
#include "engine/RenderComponent.h"

namespace engine {

auto Core::init(Context *context) -> void {
    context_ = context;
    root_ = GameObject();
}

void startGameObject(GameObject &gameObject) {
    gameObject.startComponents();
    for (auto &child : gameObject.getChildren()) {
        startGameObject(*child);
    }
}

void Core::start() { startGameObject(root_); }

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
    while (const std::optional event = context_->window->pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            context_->window->close();
        } else if (event == std::nullopt) {
            continue;
        }
        handleEventComponents(root_, event.value(), deltaTime);
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

void Core::update(float deltaTime) { updateGameObject(root_, deltaTime); }
auto Core::getRoot() -> GameObject & { return root_; }

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

auto Core::getContext() const -> Context * { return context_; }

void Core::render(float deltaTime) {
    context_->window->clear();
    // Recursively render all game objects starting from the root
    renderGameObject(getRoot(), deltaTime);
    context_->window->display();
}

void Core::loadSceneTree(GameObject &&sceneRoot) {
    root_ = std::move(sceneRoot);
    root_.setCore(this);
}
} // namespace engine
