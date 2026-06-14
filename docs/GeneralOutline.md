# General Outline

## Framework and Language

This was developed for a university programming course, that teaches C++ and game development via the SFML libraries.
Those two points are thus fixed, for the project.

## Scope

In the end, the code base is a vehicle to run a game.
We decided on a board game to digital conversion.
The game in question is **Santorini**, a relatively simple, round based strategy game on 5 by 5 grid.
The art part is currently less important, it will consists of small sprites and probably some simple animations.
Interactions are mouse based through mouse click events.

## Tooling

For versioning and collaboration we chose to use git.
Clang was selected to help with cross-platform development and the generally more helpful compiler messages.
Clangs tidy and format help to keep code style even across contributors.
CMake and Ninja, especially with presets keep the onboarding relatively frictionless.
A weak point is the integration of the correct SFML version, that often needs more work that the other parts of the setup.

## Doc Strings

While Doxygen is an industry standard for documentation, we chose prose docstrings instead.
They render fine in the IDE and export well enough in a doxygen HTML export.
Real doxygen formatting would make an export prettier, but they render with lots of noise in common IDEs as the `@tags` are not shown as intended.
See [GameObject.h](../include/engine/GameObject.h) for examples.

## Limitations

- `GameObject::clone()` produces a detached tree — _core must be set manually
- MusicComponent shares sf::Music state across copies (no per-instance playback)
- No asset caching — identical files are loaded into separate buffers

Currently, this feels like acceptable tradeoffs.
A production engine would address each with an asset manager, but the overhead is not justified here.
