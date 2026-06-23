#pragma once

#include "engine/Component.h"
#include "engine/Sprite.h"

#include <functional>

namespace game {

class ButtonComponent : public engine::Component {
  public:
    void start() override;
    void handleEvent(const std::optional<sf::Event> &event, float deltaTime) override;
    [[nodiscard]] std::unique_ptr<engine::Component> clone() const override;

    void setOnClickCallback(std::function<void()> callback);
    void setOnHoverCallback(std::function<void()> callback);

  private:
    bool isHovered{false};
    engine::Rect _buttonBounds{};
    std::function<void()> _onClickCallback;
    std::function<void()> _onHoverCallback;

    [[nodiscard]] bool isPointInRect(int x, int y, const engine::Rect &rect) const;
};

} // namespace game