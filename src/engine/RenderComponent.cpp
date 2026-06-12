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
    owner->getCore()->getContext()->window->draw(_sprite.getNative());
}

void RenderComponent::setTexture(const std::shared_ptr<sf::Texture> &texture) {
    _texture = texture;
    _sprite.setTexture(*_texture);
}

void RenderComponent::setTextureRect(const Rect &rect) { _textureRect = rect; }

} // namespace engine
