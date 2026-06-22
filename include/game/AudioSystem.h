#pragma once

#include "engine/GameObject.h"
#include "engine/MessageBus.h"
#include "engine/Music.h"
#include "engine/MusicComponent.h"
#include "engine/SoundComponent.h"
#include "game/GameMessages.h"

namespace game {
class AudioSystem {
  public:
    void setup(engine::GameObject *owner, engine::MessageBus *bus) {
        _messageBus = bus;
        setupSound<game::WorkerMovedMessage>(owner, "assets/audio/move_or_place_figure.mp3",
                                             _moveSoundObject, _workerMovedConnection);
        setupSound<game::BuildingPlacedMessage>(owner, "assets/audio/build.mp3", _buildSoundObject,
                                                _buildingPlacedConnection);
        setupSound<game::InvalidClickMessage>(owner, "assets/audio/click_on_non_clickable_tile.mp3",
                                              _invalidClickSoundObject, _invalidClickConnection);
        setupSound<game::WinMessage>(owner, "assets/audio/win.mp3", _winSoundObject,
                                     _winConnection);

        auto music = std::make_unique<engine::GameObject>();
        auto musicBuffer = std::make_shared<engine::Music>("assets/audio/ambience.mp3");
        auto musicComponent = music->addComponent<engine::MusicComponent>(musicBuffer);
        musicComponent->setRepeating(true);
        musicComponent->play();
        _ambienceMusicObject = music.get();
        owner->addChild(std::move(music));
    }

  private:
    engine::MessageBus *_messageBus{nullptr};
    engine::Connection _workerMovedConnection{};
    engine::Connection _buildingPlacedConnection{};
    engine::Connection _invalidClickConnection{};
    engine::Connection _winConnection{};

    engine::GameObject *_moveSoundObject{nullptr};
    engine::GameObject *_buildSoundObject{nullptr};
    engine::GameObject *_invalidClickSoundObject{nullptr};
    engine::GameObject *_ambienceMusicObject{nullptr};
    engine::GameObject *_winSoundObject{nullptr};

    template <typename MessageType>
    void setupSound(engine::GameObject *owner, const std::string &filePath,
                    engine::GameObject *&soundObject, engine::Connection &connection) {
        auto sound = std::make_unique<engine::GameObject>();
        auto soundBuffer = std::make_shared<engine::SoundBuffer>(filePath);
        auto soundComponent = sound->addComponent<engine::SoundComponent>(soundBuffer);
        soundObject = sound.get();
        auto *capturedSoundObject = soundObject;
        connection =
            _messageBus->subscribe<MessageType>([capturedSoundObject](MessageType message) {
                capturedSoundObject->getComponent<engine::SoundComponent>()->play();
            });
        owner->addChild(std::move(sound));
    }
};

} // namespace game
