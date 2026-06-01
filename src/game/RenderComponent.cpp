#include "game/RenderComponent.h"
#include "game/GameObject.h"
#include <SFML/System/Angle.hpp>

void RenderComponent::render(float deltaTime) {
    auto transform = owner->getWorldTransform();
    _sprite.setPosition(transform.position);
    _sprite.setRotation(sf::degrees(transform.rotation));
    _sprite.setScale({transform.scale.x, transform.scale.y});
}
void RenderComponent::setTexture(const sf::Texture &texture) { _sprite.setTexture(texture); }
void RenderComponent::setTextureRect(const Rect &rect) { _textureRect = rect; }
RenderComponent::~RenderComponent() = default;