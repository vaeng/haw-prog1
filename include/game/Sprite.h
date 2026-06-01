#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <game/Vector2.h>

class Sprite : public sf::Sprite {
  public:
    Sprite(const sf::Texture &texture) : sf::Sprite(texture) {}

    void setPosition(const Vector2 &position) { sf::Sprite::setPosition({position.x, position.y}); }

    auto getPosition() const -> Vector2 {
        auto pos = sf::Sprite::getPosition();
        return {.x = pos.x, .y = pos.y};
    }
};