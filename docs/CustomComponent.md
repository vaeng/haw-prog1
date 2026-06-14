# Creating a Custom Component

1. Inherit from `engine::Component`
2. Override the virtual methods you need: `start()`, `update(dt)`, `handleEvent(event, dt)`
3. Implement `clone()` — return `std::make_unique<YourComponent>(*this)` or manual construction

When cloning, pay attention to pointer semantics: shared pointers remain shared, raw pointers (like _render) must be re-established in start().
After cloning, call startComponents() on the new owner to re-initialize component state.

## Example Component Code

```cpp
#pragma once

#include <memory>

#include "engine/Component.h"

namespace game {

class GameObject;

/// A component that disables its owner after a certain amount of time has passed
class TimedDisableComponent : public Component {
  public:
    TimedDisableComponent(float lifeTime) : _secondsLeft(lifeTime) {}

    float getRemainingSeconds() const { return _secondsLeft; }

    void update(float deltaTime) override {
        _secondsLeft -= deltaTime;
        if (_secondsLeft <= 0) {
            owner->enabled = false;
        }
    }

    [[nodiscard]] std::unique_ptr<Component> clone() const override {
        // Copy the remaining time — the clone will self-disable after the same delay
        auto cloned = std::make_unique<TimedDisableComponent>(_secondsLeft);
        return cloned;
    }

  private:
    float _secondsLeft;
};
} // namespace game
```
