#pragma once

#include "engine/GameObject.h"
#include "engine/MessageBus.h"

namespace game {
class AudioSystem {
  public:
    void setup(engine::GameObject *owner, engine::MessageBus *bus) { _messageBus = bus; }

  private:
    engine::MessageBus *_messageBus{nullptr};
    engine::Connection _workerMovedConnection{};
    engine::Connection _buildingPlacedConnection{};

    engine::GameObject *_moveSoundObject{nullptr};
    engine::GameObject *_buildSoundObject{nullptr};
    engine::GameObject *_ambienceMusicObject{nullptr};
};

} // namespace game