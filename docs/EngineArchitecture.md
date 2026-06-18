# Thoughts about the Engine Architecture

## Coding Paradigm

We discussed object-oriented, functional and data-oriented programming.
The scale of the project does not justify the complications of the latter.
There is no performance bottleneck with the envisioned scoped, that asks for an entity system to leverage locality.
A functional approach might work, but does not fit the style of C++ and SFML as good as Racket's "How do design programs" path.
*Object-oriented design* was selected and seems like a good fit.
Not using any objects at all and having the game logic mixed with SFML calls in a single monolith file could work for our scope, but seems harder to reason and maintain.

## World Hierarchy

Next came the decision about the layout of our objects.
While a flat layout is easy to program, our game would benefit from some nesting to arrange objects.
The objects are not moving through the scene, they are mostly static.
Additionally we will probably have no more than 100 objects at any given moment.
Some quadtree layout is therefore considered overkill.
To have a general structure that helps with grouping, we selected a *scene graph hierarchy*.
There is a root node, and all objects are parented to it through other nodes.
The graph is double-linked, to ease traversal.
Children know their parent and vice-versa.

## Behaviour Coupling

We asked ourselves, where the behavior of objects would live.
Two routes came to mind: decoupled in systems and directly coupled in each object.
While a decoupled system architecture would scale better for performance, an *attached behavior* is easier to map and performant enough for our purpose.

## Inheritance vs. Composition

While classic C++ is tightly connected to inheritance, newer design philosophies tend towards composition.
We do not have a deeply nested inheritance in mind for our game.
There are not as many types to consider as an RPG or an RTS would require.
Inheritance would be fine for our project.
To be flexible for future projects and as a learning experience, we decided for a *GameObject-Components* architecture.
We want to be able to add and remove components dynamically during runtime.
This is more complicated and has more overhead, but it helps to understand design decisions that are made in industry relevant engines like Unity, Godot and, to some extend, Unreal's actor model.

## Memory Management

We decided to avoid manual memory management (de-)allocations and rely on smart pointers instead.
While there is some overhead compared to raw pointers, we take the limited garbage collection capabilities and the added safety against memory related issues.

## Clean break between Game and Engine

To make the engine re-usable we want to divide the game from the engine.
For us, that means that general purpose parts live in an engine namespace and library.
That encompasses logic, that can be re-used as it for a wide spread of games and genres, e.g. GameObjects and Transforms.
Logic, that is specific to the game, will be moved to the game side of the codebase.
As a side-effect, this helps with compilation times, as code changes in the game logic do not trigger a rebuild of the engine libraries.
For our scope this effect will be pretty minimal.

## Minimal Functionality

Do get started, the necessary parts on the engine side are relatively few: transforms, components, GameObjects, and textures.
To make the game more interesting we add sound, music, and simple sprite sheet animations.
The nature of components make it easy to write custom code on the game side, that plugs into the existing system.

## SFML Wrapping

Instead of spreading SFML dependencies all over the project, we decided on a thin abstraction layer.
Any changes in SFML would be contained to a few places without much propagation.
While future changes from SFML to SDL or GLFW are possible and easier with the wrapping approach, they are unlikely.

## Design Pattern usage

There are a bunch of design patterns, that offer solid scalability in exchange for some complexity.
As we develop our engine, we will document our reasons for using or not integrating them here.

### Dirty Transforms

Currently, world transformed are re-calculated for every frame, for every object with a render component and possible for mouse interactions.
The depth of the expected scene graph hierarchy is shallow, but due to the recursive nature of the call, many calculations are repeated.
While dirty transforms would reduce the total compute, complexity would increase significantly.
This increases the risk of propagation errors, stale data or missed update calls.
An alternative is a small "per frame cache" flag, still recalculating all world positions for every frame, but just once.

### Separated Update, Render and EventHandling Queues

Currently, all game objects and their components are traversed in the scene graph for the update, render and event handling paths.
As we have a low level of objects on the game side, it seems overly complex to add extra queues for each of them.
If any new objects are created, enabled, disabled or removed during the game loop, we would have to take care of the different queues.
While this would help performance, we do not see a bottleneck there yet.
We might decide in the future to do some on-the-fly bucket sorting during discovery for every frame, to avoid calling empty functions.
Especially the handle event loop is needed for few relevant objects, but calls all components.

### Render Queue Sorting

Discovery of the render components is following a depth-first approach in the current iteration of the engine.
To arrange and overlaps elements in the correct way, designers would need to plan the hierarchy according to the rendering order.
To decouple the scene graph structure from the rendering, we introduced layers and zIndexes.

Lower numbered layers are rendered first, while keeping a sub-order by zIndex.
In the case of overlapping objects with the same layer and zIndex, we need a stable sort, to avoid flickering.

`std::stable_sort` is an obvious candidate for the sorting process.
There are faster algorithms like heap, bucket, and radix sorting, but the scale of our intended game does not give those implementations a significant performance boost.
Additionally, we would need to add further flags to avoid z-Fighting, as some of those techniques are not stable out of the box.

### Message Bus

The message bus was integrated to reduce coupling.
For the scope of the game a string-based system would be enough, but spelling errors might produce bugs.
Instead, the implemented version features a templated publish and subscribe system.
The events can be defined by the game side of the code and do not have to be known by the engine.
Subscribers hand over the handle function on subscription, the call is thus handled automatically.
RAII is used to take care of unsubscribing via a connection object.
