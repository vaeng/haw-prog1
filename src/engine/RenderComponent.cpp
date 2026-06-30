#include <SFML/System/Angle.hpp>

#include "engine/Core.h"
#include "engine/GameObject.h"
#include "engine/RenderComponent.h"

namespace engine {

void RenderComponent::render(float deltaTime) {
    auto transform = owner->getWorldTransform();

    _sprite.setPosition(transform.position);
    _sprite.setRotation(transform.rotation);
    _sprite.setScale(transform.scale);

    owner->getCore()->getContext().window->draw(_sprite.getNative());
}

void RenderComponent::setTexture(const std::shared_ptr<Texture> &texture) {
    _texture = texture;
    _sprite.setTexture(*_texture);
}

void RenderComponent::setPivot(const Vector2 &pivot) {
    _pivot = pivot;
    _sprite.setPivot(_pivot);
}

Vector2 RenderComponent::getPivot() const { return _pivot; }

void RenderComponent::setTextureRect(const Rect &rect) {
    _textureRect = rect;
    if (_textureRect.width > 0 && _textureRect.height > 0) {
        _sprite.setTextureRect(_textureRect);
    }
    _sprite.setPivot(_pivot);
}

Rect RenderComponent::getTextureRect() const { return _textureRect; }

void RenderComponent::setTint(Color tintColor) { _sprite.setTint(tintColor); }

Rect RenderComponent::getLocalBounds() const { return _sprite.getBounds(); }

std::unique_ptr<Component> RenderComponent::clone() const {
    auto cloned = std::make_unique<RenderComponent>(_texture, layer, zIndex, _pivot);
    cloned->_sprite = _sprite;
    cloned->_textureRect = _textureRect;
    return cloned;
}

} // namespace engine
