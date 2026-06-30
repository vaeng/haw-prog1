#pragma once

#include "engine/UUID.h"

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

namespace engine {

class Texture {
  public:
    /// The unique identifier of the Texture
    UUID id{};
    Texture() = default;

    explicit Texture(const std::string &filePath, UUID id = UUID::generate()) : id(id) {
        setLoadTexture(filePath);
    }

    void setLoadTexture(const std::string &filePath) {
        if (!_texture.loadFromFile(filePath)) {
            throw std::runtime_error("Failed to load texture from " + filePath);
        }
        _filePath = filePath;
    }

    auto setRepeated(bool repeated) -> void { _texture.setRepeated(repeated); }

    void setSmooth(bool smooth) { _texture.setSmooth(smooth); }

    [[nodiscard]] const std::string &getFilePath() const { return _filePath; }

    [[nodiscard]] const sf::Texture &getNative() const { return _texture; }

  private:
    std::string _filePath;
    sf::Texture _texture;
};

} // namespace engine