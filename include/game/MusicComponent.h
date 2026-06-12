#pragma once

#include "game/Component.h"
#include <SFML/Audio/Music.hpp>
#include <memory>

class GameObject;

class MusicComponent : public Component {
  public:
    MusicComponent(const std::shared_ptr<sf::Music> &music) : _music(music) {}

    void setMusic(const std::shared_ptr<sf::Music> music) { _music = music; }

    void play() {
        if (_music) {
            _music->play();
        }
    }

    void stop() {
        if (_music) {
            _music->stop();
        }
    }

    void pause() {
        if (_music) {
            _music->pause();
        }
    }

    void setPosition(float seconds) {
        if (_music) {
            _music->setPlayingOffset(sf::seconds(seconds));
        }
    }

    void advance(float seconds) {
        if (_music) {
            _music->setPlayingOffset(_music->getPlayingOffset() + sf::seconds(seconds));
        }
    }

  private:
    std::shared_ptr<sf::Music> _music;
};