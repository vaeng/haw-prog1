#include "game/ButtonComponent.h"
#include "engine/GameObject.h"
#include "engine/RenderComponent.h"

namespace game {

void ButtonComponent::start() {
    auto position = owner->getWorldTransform().position;
    auto scale = owner->getWorldTransform().scale;
    // pivot default is 0.5, 0.5, so the position is the center of the button
    auto renderComponent = owner->getComponent<engine::RenderComponent>();
    if (renderComponent) {
        auto textureRect = renderComponent->getTextureRect();
        _buttonBounds.left = position.x - (textureRect.width * scale.x) / 2;
        _buttonBounds.top = position.y - (textureRect.height * scale.y) / 2;
        _buttonBounds.width = textureRect.width * scale.x;
        _buttonBounds.height = textureRect.height * scale.y;
    }
}

bool ButtonComponent::isPointInRect(int x, int y, const engine::Rect &rect) const {
    return x >= rect.left && x <= rect.left + rect.width && y >= rect.top &&
           y <= rect.top + rect.height;
}

void ButtonComponent::handleEvent(const std::optional<sf::Event> &event, float deltaTime) {
    if (const auto *mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
        if (isPointInRect(mouseMoved->position.x, mouseMoved->position.y, _buttonBounds)) {
            if (_onHoverCallback && !isHovered) {
                _onHoverCallback();
                isHovered = true;
            }
        } else {
            isHovered = false;
        }
    } else if (const auto *mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
        if (isPointInRect(mouseButtonPressed->position.x, mouseButtonPressed->position.y,
                          _buttonBounds)) {
            if (_onClickCallback) {
                _onClickCallback();
            } else {
                printf("Button clicked, but no callback set!\n");
            }
        } else {
            printf("Mouse button pressed outside button bounds.\n");
        }
    }
}

void ButtonComponent::setOnClickCallback(std::function<void()> callback) {
    _onClickCallback = std::move(callback);
}

void ButtonComponent::setOnHoverCallback(std::function<void()> callback) {
    _onHoverCallback = std::move(callback);
}

std::unique_ptr<engine::Component> ButtonComponent::clone() const {
    auto cloned = std::make_unique<ButtonComponent>();
    cloned->_onClickCallback = _onClickCallback;
    cloned->_onHoverCallback = _onHoverCallback;
    return cloned;
}

} // namespace game