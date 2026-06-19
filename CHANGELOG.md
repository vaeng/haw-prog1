# Changelog

## Game

### [Unreleased]

#### Added

- Turn-based state machine: Place, Select, Move, Build, (repeat)
- Multi-worker support (2 workers per player, configurable)
- Building levels: None, Level1, Level2, Level3, Dome
- Tile highlights: green (can build), blue (can move), red (blocked)
- Status labels showing current phase and active player
- Win detection (standing on Level3) with restart (R key)
- UI labels via texture-atlas spritesheet (`text.png`)

#### Fixed

- Click coordinate system: always with grid coords (-2..2)
  (was: click handler used 0..4 while tiles used -2..2)
- Worker placement no longer advances phase when clicking occupied tile

#### Changed

- Extracted InputSystem: mouse click/hover → TileClickedMessage/TileHoveredMessage
- Extracted TurnSystem: game rules/logic from GameManager, fully decoupled from engine types
- Extracted GameView: rendering ownership, tile highlights, worker positions
- Introduced MessageBus: pub/sub decoupling between systems/view/input
- WorkerData::object removed: replaced with int id + GameView::_workerObjects map
- GameManager reduced to thin orchestrator: translates SFML events to messages, wires subsystems
- `_boardTopLeft` now computed from actual world transform
  (was: hardcoded to window center)
- Scene creation simplified: board creates its own tiles programmatically
  (was: set-up from the scene and connected by position tricks)

#### Known Issues

- Building level texture rect offsets hardcoded to specific tileset layout
- Label positions tied to the board scaling

## Engine Changelog

### [Unreleased]

#### Added

- `RenderComponent::setTint(Color)`: per-sprite tinting for highlights and state
- `Sprite::setTint(Color)`: underlying implementation
- Layer and zIndex sorting in render pipeline via `std::stable_sort`
- `GameObject::clone()`: deep-copy with detached components
- `Component::clone()`: virtual clone interface (all components must implement)
- `MessageBus`: accessible via `Core::getContext()`

#### Changed

- Render pipeline now collects all components first, sorts, then renders
  (was: immediate recursive render in tree order)
- `Sprite::setPivot` uses `getTextureRect()` instead of `getLocalBounds()`
  (fix: pivot now based on visible sub-rect, not full texture)

#### Removed

- (nothing yet)

#### Notes

- SFML wrapping layer is intentionally thin: swap to SDL would touch ~6 files
- No dirty-transform caching yet; world transforms recomputed every frame
