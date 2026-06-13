#pragma once

#include <memory>

#include "engine/Component.h"
#include "engine/GameObject.h"
#include "engine/RenderComponent.h"

namespace engine {

/// Describes a frame rectangle on a sprite sheet.
///
/// The animation reads frames left-to-right, then top-to-bottom.
/// Padding values separate adjacent frames on the sheet.
struct FrameInfo {
    float framesPerSecond; /// How many frames to display per second
    int totalFrames; /// Total number of frames in the animation, assuming all rows except possibly
                     /// the last one are full
    int width;       /// Width of each frame in pixels
    int height;      /// Height of each frame in pixels
    int verticalOffset;   /// Vertical position of the top left corner of the first frame in the
                          /// animation sheet
    int horizontalOffset; /// Horizontal position of the top left corner of the first frame in the
                          /// animation sheet
    int horizontalFrameCount; /// Number of frames in each row
    int verticalFrameCount;   /// Number of rows in the animation sheet
    int horizontalPadding;    /// Padding between frames horizontally
    int verticalPadding;      /// Padding between frames vertically
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

    [[nodiscard]] std::unique_ptr<Component> clone() const override {
        auto cloned = std::make_unique<AnimationComponent>(_frameInfo);
        cloned->_currentFrame = _currentFrame;
        cloned->_timeSinceLastFrame = _timeSinceLastFrame;
        return cloned;
    }

  private:
    RenderComponent *_render{nullptr};
    FrameInfo _frameInfo{};
    int _currentFrame{};
    float _timeSinceLastFrame{};
};
} // namespace engine
