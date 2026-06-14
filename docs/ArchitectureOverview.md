# Architecture Overview

A game starts by constructing a Core with a window config, then loading a GameObject tree as the scene.
`Core::start()` calls `Component::start()` on every component in the tree.
`Core::run()` enters the loop: handle events, update, render — all dispatched recursively through the scene graph.
Each step calls the corresponding virtual method on every component, as shown in the following diagram.

 All engine types can be found in `include/engine/`. See [Core.h](../include/engine/Core.h) for the game loop API.

```mermaid
sequenceDiagram
    participant M as main()
    participant C as Core
    participant G as GameObject
    participant Com as Component

    M->>C: Core(config)
    M->>C: loadScene(root)
    M->>C: start()
    C->>G: startComponents (recursive)
    G->>Com: start()
    M->>C: run()
    loop each frame
        C->>C: handleEvents()
        C->>G: handleEventComponents (recursive)
        G->>Com: handleEvent(event, dt)
        C->>C: update()
        C->>G: updateComponents (recursive)
        G->>Com: update(dt)
        C->>C: render()
        C->>C: collect RenderComponents
        C->>C: sort by (layer, zIndex)
        C->>Com: render(dt) [sorted order]
    end
```

The next diagram shows the structural relationship between the core, game objects and their components.

```mermaid
classDiagram
    class Core {
        -RenderWindow window
        -GameObject root
        +start()
        +run()
    }
    class GameObject {
        +Transform localTransform
        +bool enabled
        +addChild()
        +addComponent~T~()
        +getComponent~T~()
        +clone()
    }
    class Component {
        +bool enabled
        +GameObject* owner
        +start()
        +update(dt)
        +handleEvent(event, dt)
        +clone()*
    }
    class RenderComponent {
        +int layer
        +int zIndex
        +render(dt)
    }
    class AnimationComponent
    class MusicComponent
    class SoundComponent

    Core --> "1" GameObject : root
    GameObject --> "*" GameObject : children
    GameObject --> "*" Component : components
    Component <|-- RenderComponent
    Component <|-- AnimationComponent
    Component <|-- MusicComponent
    Component <|-- SoundComponent
```
