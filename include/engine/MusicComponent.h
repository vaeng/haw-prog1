#pragma once

#include <SFML/Audio/Music.hpp>
#include <memory>

#include "engine/Component.h"
#include "engine/Music.h"

namespace engine {

class GameObject;

class MusicComponent : public Component {
  public:
    MusicComponent(const std::shared_ptr<Music> &music) : _music(music) {}

    void setMusic(const std::shared_ptr<Music> &music) { _music = music; }

    void play() {
        if (_music) {
            _music->getNative().play();
        }
    }

    void stop() {
        if (_music) {
            _music->getNative().stop();
        }
    }

    void pause() {
        if (_music) {
            _music->getNative().pause();
        }
    }

    void setPosition(float seconds) {
        if (_music) {
            _music->getNative().setPlayingOffset(sf::seconds(seconds));
        }
    }

    void advance(float seconds) {
        if (_music) {
            _music->getNative().setPlayingOffset(_music->getNative().getPlayingOffset() +
                                                 sf::seconds(seconds));
        }
    }

    /// Gets a clone of the current MusicComponent, with the same music-resource
    ///
    /// Note: Both object will control the resource with playing, pausing, and stopping it, so they
    /// would affect each other.
    [[nodiscard]] std::unique_ptr<Component> clone() const override {
        auto cloned = std::make_unique<MusicComponent>(_music);
        return cloned;
    }

  private:
    std::shared_ptr<Music> _music;
};
} // namespace engine
