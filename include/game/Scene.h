#pragma once

#include "engine/GameObject.h"

namespace game {

[[nodiscard]] auto createScene() -> engine::GameObject;
[[nodiscard]] auto gameScene() -> engine::GameObject;
[[nodiscard]] auto mainScene() -> engine::GameObject;
[[nodiscard]] auto exampleWithRick() -> engine::GameObject;

} // namespace game