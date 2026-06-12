#include "game/Game.h"
#include "game/Component.h"
#include "game/RenderComponent.h"
#include <SFML/Window/Event.hpp>

Game *Game::instance_ = nullptr;

auto Game::instance() -> Game & {
    if (instance_ == nullptr) {
        instance_ = new Game();
    }
    return *instance_;
}

auto Game::init(Context *context) -> void {
    context_ = context;
    root_ = GameObject();
}

void startGameObject(GameObject &gameObject) {
    gameObject.startComponents();
    for (auto &child : gameObject.getChildren()) {
        startGameObject(*child);
    }
}

void Game::start() { startGameObject(root_); }

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

void Game::handleEvents(float deltaTime) {
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

void Game::update(float deltaTime) { updateGameObject(root_, deltaTime); }
auto Game::getRoot() -> GameObject & { return root_; }

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

auto Game::getContext() const -> Context * { return context_; }

void Game::render(float deltaTime) {
    context_->window->clear();
    // Recursively render all game objects starting from the root
    renderGameObject(getRoot(), deltaTime);
    context_->window->display();
}

void Game::loadSceneTree(GameObject &&sceneRoot) { root_ = std::move(sceneRoot); }
