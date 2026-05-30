#include "game/Game.h"
#include "game/Component.h"
#include "game/RenderComponent.h"

void Game::handleInput() {}
void Game::update(double deltaTime) {}
auto Game::getRoot() -> GameObject & { return root_; }

void renderGameObject(GameObject &gameObject, double deltaTime) {
    if (!gameObject.enabled) {
        return;
    }

    // Render the current game object
    for (auto *component : gameObject.getComponents()) {
        if (component->enabled) {
            if (auto *renderComponent = dynamic_cast<RenderComponent *>(component)) {
                renderComponent->render();
            }
        }
    }

    // Recursively render children
    for (auto *child : gameObject.getChildren()) {
        renderGameObject(*child, deltaTime);
    }
}

void Game::render(double deltaTime) {
    // recursivly render all game objects starting from the root
    renderGameObject(getRoot(), deltaTime);
}