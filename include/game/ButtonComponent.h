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
    void setOnReleaseCallback(std::function<void()> callback);
    void setOnHoverCallback(std::function<void()> callback);
    void setOnMouseExitCallback(std::function<void()> callback);

    void setButtonBounds(engine::Rect bounds);
    [[nodiscard]] const engine::Rect &getButtonBounds() const;

  private:
    bool isHovered{false};
    engine::Rect _buttonBounds{};
    engine::Rect _customBounds{};
    std::function<void()> _onClickCallback;
    std::function<void()> _onHoverCallback;
    std::function<void()> _onReleaseCallback;
    std::function<void()> _onMouseExitCallback;

    [[nodiscard]] bool isPointInRect(int x, int y) const;
};

} // namespace game