#include "game/RenderComponent.h"
#include "game/Game.h"
#include "game/GameObject.h"
#include <SFML/System/Angle.hpp>

void RenderComponent::render(float deltaTime) {
    auto transform = owner->getWorldTransform();
    _sprite.setPosition(transform.position);
    _sprite.setRotation(transform.rotation);
    _sprite.setScale(transform.scale);
    _sprite.setTextureRect(_textureRect);
    owner->getGame()->getContext()->window->draw(_sprite.getNative());
}

void RenderComponent::setTexture(const std::shared_ptr<sf::Texture> &texture) {
    _texture = texture;
    _sprite.setTexture(*_texture);
}

void RenderComponent::setTextureRect(const Rect &rect) { _textureRect = rect; }
