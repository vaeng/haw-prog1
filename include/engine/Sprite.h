
#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Angle.hpp>
#include <cstdint>

#include "engine/Texture.h"
#include "engine/Vector2.h"
namespace engine {

struct Rect {
    int left;
    int top;
    int width;
    int height;
};

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a = 255;
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

    auto setRotation(float angle, bool inDegrees = false) -> void {
        if (_sprite) {
            if (inDegrees) {
                _sprite->setRotation(sf::degrees(angle));
            } else {
                _sprite->setRotation(sf::radians(angle));
            }
        }
    }

    auto setScale(const Vector2 &scale) -> void {
        if (_sprite) {
            _sprite->setScale({scale.x, scale.y});
        }
    }

    auto setPivot(const Vector2 &pivot) -> void {
        if (_sprite) {
            auto bounds = _sprite->getTextureRect();
            _sprite->setOrigin({bounds.size.x * pivot.x, bounds.size.y * pivot.y});
        }
    }
    /// Set the tint color of the sprite. This will multiply.
    ///
    /// To reset the tint, set it to white (255, 255, 255, 255).
    auto setTint(Color tintColor) -> void {
        if (_sprite) {
            _sprite->setColor({tintColor.r, tintColor.g, tintColor.b, tintColor.a});
        }
    }

    auto getBounds() const -> Rect {
        if (!_sprite) {
            return {.left = 0, .top = 0, .width = 0, .height = 0};
        }
        auto bounds = _sprite->getLocalBounds();
        return {.left = static_cast<int>(bounds.position.x),
                .top = static_cast<int>(bounds.position.y),
                .width = static_cast<int>(bounds.size.x),
                .height = static_cast<int>(bounds.size.y)};
    }

    auto getNative() -> sf::Sprite & { return *_sprite; }

  private:
    std::optional<sf::Sprite> _sprite;
};
} // namespace engine
