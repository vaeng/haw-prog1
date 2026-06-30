#pragma once

#include "engine/UUID.h"
#include <SFML/Audio.hpp>

namespace engine {

class Music {
  public:
    UUID id{UUID::generate()};
    Music() = default;

    explicit Music(const std::string &filePath) { setLoadMusic(filePath); }

    void setLoadMusic(const std::string &filePath) {
        if (!_music.openFromFile(filePath)) {
            throw std::runtime_error("Failed to load music from " + filePath);
        }
        _filePath = filePath;
    }

    [[nodiscard]] const std::string &getFilePath() const { return _filePath; }

    [[nodiscard]] const sf::Music &getNative() const { return _music; }
    [[nodiscard]] sf::Music &getNative() { return _music; }

  private:
    sf::Music _music;
    std::string _filePath; // Store the file path for serialization
};

} // namespace engine
