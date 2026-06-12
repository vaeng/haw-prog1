#pragma once

#include <SFML/Audio.hpp>

namespace engine {

class Music {
  public:
    Music() = default;

    explicit Music(const std::string &filePath) {
        if (!_music.openFromFile(filePath)) {
            throw std::runtime_error("Failed to load music from " + filePath);
        }
    }

    [[nodiscard]] const sf::Music &getNative() const { return _music; }
    [[nodiscard]] sf::Music &getNative() { return _music; }

  private:
    sf::Music _music;
};

} // namespace engine
