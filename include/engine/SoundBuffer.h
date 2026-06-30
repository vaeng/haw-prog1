#pragma once

#include <SFML/Audio.hpp>

#include "engine/UUID.h"

#include <nlohmann/json.hpp>
#include <string>

namespace engine {

class SoundBuffer {
  public:
    /// The unique identifier of the SoundBuffer
    UUID id{UUID::generate()};

    SoundBuffer() = default;

    explicit SoundBuffer(const std::string &filePath) { setLoadSoundBuffer(filePath); }

    void setLoadSoundBuffer(const std::string &filePath) {
        _filePath = filePath;
        if (!_soundBuffer.loadFromFile(filePath)) {
            throw std::runtime_error("Failed to load sound buffer from " + filePath);
        }
    }

    [[nodiscard]] const std::string &getFilePath() const { return _filePath; }

    [[nodiscard]] const sf::SoundBuffer &getNative() const { return _soundBuffer; }
    [[nodiscard]] sf::SoundBuffer &getNative() { return _soundBuffer; }

  private:
    std::string _filePath;
    sf::SoundBuffer _soundBuffer;
};
} // namespace engine
