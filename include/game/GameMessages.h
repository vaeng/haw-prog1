#pragma once

namespace game {

// Message published when mouse is clicked.
struct MouseClickedMessage {
    int windowX;
    int windowY;
};

// Message published when mouse is moved.
struct MouseMovedMessage {
    int windowX;
    int windowY;
};

/// Published when a tile is clicked. x/y are grid coordinates.
struct TileClickedMessage {
    int x;
    int y;
};

/// Published when the mouse hovers over a tile. x/y are grid coordinates.
struct TileHoveredMessage {
    int x;
    int y;
};

/// Published after any game state change (move, build, placement, restart, etc.)
/// The View reacts to this by updating labels, highlights, and worker positions.
struct StateChangedMessage {};

// Published when the game should be restarted
struct RestartGameMessage {};

} // namespace game
