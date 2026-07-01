---
marp: true
theme: gaia
class: lead
paginate: true
style: |
  section {
    background: linear-gradient(160deg, #faf5eb 0%, #fdf8f0 30%, #faf0e6 100%);
    color: #2a2d35;
    font-family: "Segoe UI", "Helvetica Neue", Arial, sans-serif;
  }
  section::before {
    content: "";
    position: absolute;
    bottom: 16px;
    left: 16px;
    width: 40px;
    height: 40px;
    background: url("../assets/textures/Game-Icon.png") no-repeat center / contain;
    opacity: 0.25;
  }
  h1 { color: #e6584a; font-weight: 800; letter-spacing: -0.02em; }
  h2 { color: #e6584a; }
  strong { color: #1e2024; }
  em { color: #e6584a; }
  a { color: #e6584a; }
  pre {
    background: #f0ebe3 !important;
    border: 1px solid #d9d0c4;
    border-radius: 8px;
    padding: 16px 20px !important;
    line-height: 1.4;
  }
  pre code {
    font-family: "Courier New", "Roboto Mono", "Liberation Mono", monospace !important;
    font-size: 24px !important;
    background: none;
    padding: 0;
  }
  code {
    background: #f0ebe3;
    color: #e6584a;
    padding: 2px 6px;
    border-radius: 4px;
    font-size: 0.85em;
  }
  blockquote {
    background: rgba(230, 88, 74, 0.08);
    border-left: 4px solid #e6584a;
    padding: 10px 18px;
    border-radius: 0 8px 8px 0;
  }
  table { border-collapse: collapse; width: 100%; }
  th {
    background: #e8e0d6;
    color: #e6584a;
    padding: 8px 14px;
    text-align: left;
    font-weight: 700;
  }
  td { padding: 6px 14px; border-bottom: 1px solid #d9d0c4; }
  tr:nth-child(even) { background: rgba(232, 224, 214, 0.4); }
  section.lead {
    background: linear-gradient(160deg, #fcf3e8 0%, #fae6d8 50%, #f8dcc8 100%) !important;
    display: flex;
    flex-direction: column;
    justify-content: center;
    text-align: center;
  }
  section.lead h1 {
    color: #e6584a;
    text-shadow: 0 0 40px rgba(230, 88, 74, 0.15);
  }
  section.lead p { color: #5a5e66; }
  section::after { color: rgba(42, 45, 53, 0.25); }
---

# Santorini

**Caroline Römer** & **Christian Willner**

Game Programming 1 [INF.26S]

---

# What we built

A custom 2D game engine with the game Santorini on top.

&nbsp;

**Course**: C++23, SFML 3, 12 weeks
**Engine**: scene graph, components, message bus (game-agnostic)
**Game**: Santorini - place workers, move, build, win at level 3

---

# Why we wrote the engine

Just use SFML to write everything in `main()`, but:

we wanted to understand the layer between the framework and the game: game loop, scene graph traversal, memory model, and decoupling patterns.

The engine is **game-agnostic**. The Santorini logic lives entirely on top.

---

# Model / System / View

We split a god-class into four pieces that talk through a message bus.

**MessageBus**: routes events between systems

| System | Role |
|---|---|
| GameStateData | Stores board and worker positions |
| TurnSystem | Enforces rules, advances turns |
| GameView | Renders through SFML |
| InputSystem | Translates mouse clicks to events |

---

# The message bus

The glue that keeps systems decoupled. Publish and subscribe without knowing each other.

| Event | Direction |
|---|---|
| `MouseClicked` | InputSystem **publishes** to MessageBus |
| `TileClicked` | TurnSystem **subscribes** from MessageBus |
| `StateChanged` | TurnSystem **publishes** to MessageBus |
| `StateChanged` | GameView **subscribes** from MessageBus |

Type-safe, templated & RAII-guarded in 25 lines of code.

---

# Components over inheritance

GameObjects own a list of Components. Attach behavior at runtime.

```cpp
board->addComponent<RenderComponent>();
worker->addComponent<AnimationComponent>();
```

Same pattern as Unity's MonoBehaviour or Godot's nodes. The engine does not need to know game logic. Devs can write new component types without touching the engine code.

---

# Memory: modern C++ only

No raw new/delete in the codebase.

| Mechanism | What it guarantees |
|---|---|
| `unique_ptr` | Each component has exactly one owner |
| `shared_ptr` | Textures, soundBuffers shared across components |
| RAII connection objects | Auto-unsubscribe from the bus on destruction |

---

# Game logic in 5 rules

A state machine that cycles per player:

| Step | Action |
|---|---|
| Place | Put workers on the board |
| Select | Choose a worker to move |
| Move | Move to adjacent tile (≤+1 height), win at level 3 |
| Build | Place block on adjacent tile |
| | Next player goes back to Select |

---

# Tradeoffs

If we had more time...

| Our choice | At scale you would... |
|---|---|
| Recalculate transforms every frame | Dirty-flag propagation |
| One file per asset, duplicate loads | Asset manager + ref counting |
| Prose comments and markdown files | Formal Doxygen docs |

---

# Live demo

Watch the message bus in action: each click fires an event through the bus, the view reacts without knowing the game logic.

1. Placement, select, move, build
2. The view updates without knowing the logic
3. Arrows and highlights driven by the message bus

---

# Key takeaways

- Custom engine: scene graph, components, message bus
- Model / System / View: clean separation, no god class
- Modern C++: smart pointers, RAII, no manual memory
- Document decisions for later usage

**Code**: [github.com/vaeng/haw-prog1](https://github.com/vaeng/haw-prog1)
**Slides**: [marp.app](https://marp.app)
