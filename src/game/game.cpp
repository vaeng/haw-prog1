#include "game/Game.h"
#include "game/Component.h"
#include "game/RenderComponent.h"

void Game::handleInput() {}
void Game::update(float deltaTime) {}
auto Game::getRoot() -> GameObject & { return root_; }

void renderGameObject(GameObject &gameObject, float deltaTime) {
    if (!gameObject.enabled) {
        return;
    }

    // Render the current game object
    for (auto *component : gameObject.getComponents()) {
        if (component->enabled) {
            if (auto *renderComponent = dynamic_cast<RenderComponent *>(component)) {
                renderComponent->render(deltaTime);
            }
        }
    }

    // Recursively render children
    for (auto *child : gameObject.getChildren()) {
        renderGameObject(*child, deltaTime);
    }
}

void Game::render(float deltaTime) {
    // recursivly render all game objects starting from the root
    renderGameObject(getRoot(), deltaTime);
}