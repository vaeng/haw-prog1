#pragma once

#include "engine/Component.h"
#include "engine/GameObject.h"
#include "engine/RenderComponent.h"

namespace engine {

struct FrameInfo {
    float framesPerSecond;
    int totalFrames;
    int left;
    int top;
    int width;
    int height;
    int verticalOffset;
    int horizontalOffset;
    int horizontalFrameCount;
    int verticalFrameCount;
    int horizontalPadding;
    int verticalPadding;
};

class AnimationComponent : public Component {
  public:
    AnimationComponent(const FrameInfo &info) : _currentFrame(0), _frameInfo(info) {}
    void start() override {
        _render = owner->getComponent<RenderComponent>();
        if (_render == nullptr) {
            throw std::runtime_error("AnimationComponent requires a RenderComponent");
        }
    }

    void update(float dt) override {
        if (_render == nullptr) {
            return;
        }
        _timeSinceLastFrame += dt;

        if (_timeSinceLastFrame >= 1.0f / _frameInfo.framesPerSecond) {
            _currentFrame = (_currentFrame + 1) % (_frameInfo.totalFrames);
            _timeSinceLastFrame = 0;
        }
        int left = (_currentFrame % _frameInfo.horizontalFrameCount) *
                       (_frameInfo.width + _frameInfo.horizontalPadding) +
                   _frameInfo.horizontalOffset;
        int top = (_currentFrame / _frameInfo.horizontalFrameCount) *
                      (_frameInfo.height + _frameInfo.verticalPadding) +
                  _frameInfo.verticalOffset;
        _render->setTextureRect(
            {.left = left, .top = top, .width = _frameInfo.width, .height = _frameInfo.height});
    }

    [[nodiscard]] auto getFrameInfo() const -> FrameInfo { return _frameInfo; }
    auto setFrameInfo(const FrameInfo &info) { _frameInfo = info; }

  private:
    RenderComponent *_render{nullptr};
    FrameInfo _frameInfo{};
    int _currentFrame{};
    float _timeSinceLastFrame{};
};
} // namespace engine
