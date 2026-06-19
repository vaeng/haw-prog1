#pragma once

#include "engine/MessageBus.h"
#include "game/GameMessages.h"
#include <tuple>

namespace game {

struct BoardProperties;

class InputSystem {
  public:
    InputSystem(const BoardProperties &boardProperties) : _boardProperties(boardProperties) {};
    void setup(engine::MessageBus *bus);

  private:
    const BoardProperties &_boardProperties;
    engine::MessageBus *_messageBus{nullptr};
    engine::Connection _mouseClickedConnection;
    engine::Connection _mouseMovedConnection;

    std::tuple<bool, int, int> getTileUnderMouse(int mouseX, int mouseY);
    void onMouseMoveEvent(const game::MouseMovedMessage &message);
    void onClickEvent(const game::MouseClickedMessage &message);
};

} // namespace game