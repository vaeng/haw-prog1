#pragma once

#include <SFML/Graphics.hpp>

namespace engine {

class Texture {
  public:
    Texture() = default;

    explicit Texture(const std::string &filePath) {
        if (!_texture.loadFromFile(filePath)) {
            throw std::runtime_error("Failed to load texture from " + filePath);
        }
    }

    [[nodiscard]] const sf::Texture &getNative() const { return _texture; }

  private:
    sf::Texture _texture;
};

} // namespace engine