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

  private:
    std::shared_ptr<Music> _music;
};
} // namespace engine
