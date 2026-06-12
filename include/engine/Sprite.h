
#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Angle.hpp>

#include "engine/Texture.h"
#include "engine/Vector2.h"
namespace engine {

struct Rect {
    int left;
    int top;
    int width;
    int height;
};

class Sprite {
  public:
    Sprite() = default;

    void setTexture(const Texture &tex) {
        _sprite.emplace(tex.getNative()); // construct Sprite ONLY when texture exists
    }

    void setTextureRect(const Rect &rect) {
        if (_sprite) {
            _sprite->setTextureRect({{rect.left, rect.top}, {rect.width, rect.height}});
        }
    }

    void setPosition(const Vector2 &position) {
        if (_sprite) {
            _sprite->setPosition({position.x, position.y});
        }
    }

    auto getPosition() const -> Vector2 {
        if (!_sprite) {
            return {.x = 0, .y = 0};
        }
        auto pos = _sprite->getPosition();
        return {.x = pos.x, .y = pos.y};
    }

    auto setRotation(float angle) -> void {
        if (_sprite) {
            _sprite->setRotation(sf::degrees(angle));
        }
    }

    auto setScale(const Vector2 &scale) -> void {
        if (_sprite) {
            _sprite->setScale({scale.x, scale.y});
        }
    }

    auto getNative() -> sf::Sprite & { return *_sprite; }

  private:
    std::optional<sf::Sprite> _sprite;
};
} // namespace engine
