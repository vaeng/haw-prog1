#pragma once

#include "game/Component.h"
#include "game/Sprite.h"
#include <memory>

class GameObject;

class RenderComponent : public Component {
  public:
    RenderComponent(const std::shared_ptr<sf::Texture> &texture) : _texture(texture) {
        _sprite.setTexture(*_texture);
    }

    void setTexture(const std::shared_ptr<sf::Texture> &texture);
    void setTextureRect(const Rect &rect);
    void render(float deltaTime);

  private:
    std::shared_ptr<sf::Texture> _texture;
    Sprite _sprite;
    Rect _textureRect;
};