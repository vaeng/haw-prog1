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
    _sprite.setTextureRect(_textureRect);
    owner->getCore()->getContext().window->draw(_sprite.getNative());
}

void RenderComponent::setTexture(const std::shared_ptr<Texture> &texture) {
    _texture = texture;
    _sprite.setTexture(*_texture);
}

void RenderComponent::setTextureRect(const Rect &rect) { _textureRect = rect; }

std::unique_ptr<Component> RenderComponent::clone() const {
    {
        auto cloned = std::make_unique<RenderComponent>(_texture);
        cloned->_sprite = _sprite;
        cloned->_textureRect = _textureRect;
        return cloned;
    }
}

} // namespace engine
