#include "game/InputSystem.h"
#include "game/GameManager.h"
#include "game/GameMessages.h"

namespace game {
std::tuple<bool, int, int> InputSystem::getTileUnderMouse(int mouseX, int mouseY) {
    if (mouseX >= _boardProperties.boardTopLeft.x &&
        mouseX <= _boardProperties.boardBottomRight.x &&
        mouseY >= _boardProperties.boardTopLeft.y &&
        mouseY <= _boardProperties.boardBottomRight.y) {
        int tileX = static_cast<int>((mouseX - _boardProperties.boardTopLeft.x) /
                                     _boardProperties.screenTileSize) -
                    _boardProperties.numTiles / 2;
        int tileY = static_cast<int>((mouseY - _boardProperties.boardTopLeft.y) /
                                     _boardProperties.screenTileSize) -
                    _boardProperties.numTiles / 2;
        return {true, tileX, tileY};
    }
    return {false, 0, 0};
}

void InputSystem::setup(engine::MessageBus *bus) {
    _messageBus = bus;
    _mouseClickedConnection = _messageBus->subscribe<MouseClickedMessage>(
        [this](const MouseClickedMessage &message) { onClickEvent(message); });
    _mouseMovedConnection = _messageBus->subscribe<MouseMovedMessage>(
        [this](const MouseMovedMessage &message) { onMouseMoveEvent(message); });
}

void InputSystem::onClickEvent(const MouseClickedMessage &event) {
    auto [clickedInBounds, tileX, tileY] = getTileUnderMouse(event.windowX, event.windowY);
    if (clickedInBounds) {
        _messageBus->publish(TileClickedMessage{.x = tileX, .y = tileY});
    }
}

void InputSystem::onMouseMoveEvent(const MouseMovedMessage &event) {
    auto [clickedInBounds, tileX, tileY] = getTileUnderMouse(event.windowX, event.windowY);
    if (clickedInBounds) {
        _messageBus->publish(TileHoveredMessage{.x = tileX, .y = tileY});
    }
}

} // namespace game