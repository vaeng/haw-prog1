#pragma once

#include <SFML/Audio/Sound.hpp>
#include <memory>

#include "engine/Component.h"
#include "engine/SoundBuffer.h"

namespace engine {

class GameObject;

class SoundComponent : public Component {
  public:
    SoundComponent(const std::shared_ptr<SoundBuffer> &soundBuffer)
        : _soundBuffer(soundBuffer), _soundInstance(sf::Sound{soundBuffer->getNative()}) {}

    void setSoundBuffer(const std::shared_ptr<SoundBuffer> &soundBuffer) {
        _soundBuffer = soundBuffer;
        _soundInstance.setBuffer(_soundBuffer->getNative());
    }

    std::shared_ptr<SoundBuffer> getSoundBuffer() const { return _soundBuffer; }

    void play() { _soundInstance.play(); }

    void stop() { _soundInstance.stop(); }

    void pause() { _soundInstance.pause(); }

    void setPosition(float seconds) { _soundInstance.setPlayingOffset(sf::seconds(seconds)); }

    void advance(float seconds) {
        _soundInstance.setPlayingOffset(_soundInstance.getPlayingOffset() + sf::seconds(seconds));
    }

    /// Gets a clone of the current SoundComponent, with the same soundBuffer
    ///
    /// Note: Each object has its own sf::Sound instance, so that they can be played independently
    [[nodiscard]] std::unique_ptr<Component> clone() const override {
        auto cloned = std::make_unique<SoundComponent>(_soundBuffer);
        return cloned;
    }

  private:
    std::shared_ptr<SoundBuffer> _soundBuffer;
    sf::Sound _soundInstance;
};
} // namespace engine
