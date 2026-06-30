#pragma once

#include <memory>

#include "engine/Component.h"
#include "engine/Sprite.h"
#include "engine/Texture.h"
#include "engine/Vector2.h"

namespace engine {

class GameObject;

class RenderComponent : public Component {
  public:
    int layer{};
    int zIndex{};

    RenderComponent(const std::shared_ptr<Texture> &texture, int layer = 0, int zIndex = 0,
                    Vector2 pivot = Vector2{0.5f, 0.5f}, bool repeated = false)
        : _texture(texture), layer(layer), zIndex(zIndex), _pivot(pivot), _repeated(repeated) {
        _sprite.setTexture(*_texture);
        _sprite.setPivot(_pivot);
        _texture->setRepeated(_repeated);
    }
    void setRepeated(bool repeated) {
        _repeated = repeated;
        if (_texture) {
            _texture->setRepeated(repeated);
        }
    }
    bool isRepeated() const { return _repeated; }
    void setPivot(const Vector2 &pivot);
    [[nodiscard]] Vector2 getPivot() const;
    void setTexture(const std::shared_ptr<Texture> &texture);
    [[nodiscard]] std::shared_ptr<Texture> getTexture() const { return _texture; }
    void setTextureRect(const Rect &rect);
    Rect getTextureRect() const;
    void render(float deltaTime);
    void setTint(Color tintColor);
    Rect getLocalBounds() const;
    [[nodiscard]] std::unique_ptr<Component> clone() const override;

  private:
    std::shared_ptr<Texture> _texture;
    Sprite _sprite;
    Rect _textureRect{};
    Vector2 _pivot{};
    bool _repeated{false};
    Vector2 _renderOffset{};
};
} // namespace engine
