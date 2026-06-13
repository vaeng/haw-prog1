#pragma once

#include <SFML/Audio.hpp>

namespace engine {

class SoundBuffer {
  public:
    SoundBuffer() = default;

    explicit SoundBuffer(const std::string &filePath) {
        if (!_soundBuffer.loadFromFile(filePath)) {
            throw std::runtime_error("Failed to load sound from " + filePath);
        }
    }

    [[nodiscard]] const sf::SoundBuffer &getNative() const { return _soundBuffer; }
    [[nodiscard]] sf::SoundBuffer &getNative() { return _soundBuffer; }

  private:
    sf::SoundBuffer _soundBuffer;
};
} // namespace engine
