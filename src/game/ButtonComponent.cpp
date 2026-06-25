#include "game/ButtonComponent.h"
#include "engine/GameObject.h"
#include "engine/RenderComponent.h"

namespace game {

void ButtonComponent::start() {
    engine::Rect boundsToUse{};
    if (_customBounds.width != 0 && _customBounds.height != 0) {
        boundsToUse = _customBounds;
    } else {
        auto renderComponent = owner->getComponent<engine::RenderComponent>();
        if (renderComponent) {
            boundsToUse = renderComponent->getTextureRect();
        }
    }
    auto position = owner->getWorldTransform().position;
    auto scale = owner->getWorldTransform().scale;
    // pivot default is 0.5, 0.5, so the position is the center of the button
    _buttonBounds.left = position.x - (boundsToUse.width * scale.x) / 2;
    _buttonBounds.top = position.y - (boundsToUse.height * scale.y) / 2;
    _buttonBounds.width = boundsToUse.width * scale.x;
    _buttonBounds.height = boundsToUse.height * scale.y;
}

bool ButtonComponent::isPointInRect(int x, int y) const {
    return x >= _buttonBounds.left && x <= _buttonBounds.left + _buttonBounds.width &&
           y >= _buttonBounds.top && y <= _buttonBounds.top + _buttonBounds.height;
}

void ButtonComponent::handleEvent(const std::optional<sf::Event> &event, float deltaTime) {

    if (const auto *mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
        if (isPointInRect(mouseMoved->position.x, mouseMoved->position.y)) {
            if (_onHoverCallback && !isHovered) {
                _onHoverCallback();
                isHovered = true;
            }
        } else {
            if (_onMouseExitCallback && isHovered) {
                _onMouseExitCallback();
                isHovered = false;
            }
        }
    } else if (const auto *mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
        if (isPointInRect(mouseButtonReleased->position.x, mouseButtonReleased->position.y)) {
            if (_onReleaseCallback) {
                _onReleaseCallback();
            }
        }
    } else if (const auto *mouseButtonClicked = event->getIf<sf::Event::MouseButtonPressed>()) {
        if (isPointInRect(mouseButtonClicked->position.x, mouseButtonClicked->position.y)) {
            if (_onClickCallback) {
                _onClickCallback();
            }
        }
    }
}

void ButtonComponent::setOnClickCallback(std::function<void()> callback) {
    _onClickCallback = std::move(callback);
}

void ButtonComponent::setOnReleaseCallback(std::function<void()> callback) {
    _onReleaseCallback = std::move(callback);
}

void ButtonComponent::setOnHoverCallback(std::function<void()> callback) {
    _onHoverCallback = std::move(callback);
}

void ButtonComponent::setOnMouseExitCallback(std::function<void()> callback) {
    _onMouseExitCallback = std::move(callback);
}

void ButtonComponent::setButtonBounds(engine::Rect bounds) { _customBounds = bounds; }

const engine::Rect &ButtonComponent::getButtonBounds() const { return _buttonBounds; }

std::unique_ptr<engine::Component> ButtonComponent::clone() const {
    auto cloned = std::make_unique<ButtonComponent>();
    cloned->_onClickCallback = _onClickCallback;
    cloned->_onReleaseCallback = _onReleaseCallback;
    cloned->_onHoverCallback = _onHoverCallback;
    cloned->_onMouseExitCallback = _onMouseExitCallback;
    cloned->_buttonBounds = _buttonBounds;
    return cloned;
}

} // namespace game