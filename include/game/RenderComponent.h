#pragma once

#include "game/Component.h"
#include "game/Sprite.h"

struct Rect {
    int left;
    int top;
    int width;
    int height;
};

class RenderComponent : public Component {
  public:
    RenderComponent(GameObject *owner);
    void setTexture(const sf::Texture &texture);
    void setTextureRect(const Rect &rect);
    ~RenderComponent();
    void render(float deltaTime);

  private:
    Sprite _sprite;
    Rect _textureRect;
};