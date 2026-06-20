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
    AnimationComponent(const FrameInfo &info = FrameInfo{}) : _currentFrame(0), _frameInfo(info) {}
    void start() override {
        _render = owner->getComponent<RenderComponent>();
        if (_render == nullptr) {
            throw std::runtime_error("AnimationComponent requires a RenderComponent");
        }
    }

    void update(float dt) override {
        if (_render == nullptr || !_isPlaying) {
            return;
        }
        _timeSinceLastFrame += dt;
        // check if it's time to advance to the next frame
        if (_timeSinceLastFrame >= 1.0f / _frameInfo.framesPerSecond) {
            _timeSinceLastFrame = 0;
            _currentFrame++;
        }
        // check if we need to loop back to the beginning of the animation
        if (_currentFrame >= _frameInfo.totalFrames) {
            if (_loop) {
                _currentFrame = 0;
            } else {
                _currentFrame = _frameInfo.totalFrames - 1; // stay on the last frame
                _isPlaying = false;                         // stop the animation
            }
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
    auto setFrameInfo(const FrameInfo &info) {
        stop();
        _frameInfo = info;
    }

    [[nodiscard]] std::unique_ptr<Component> clone() const override {
        auto cloned = std::make_unique<AnimationComponent>(_frameInfo);
        cloned->_currentFrame = _currentFrame;
        cloned->_timeSinceLastFrame = _timeSinceLastFrame;
        return cloned;
    }
    [[nodiscard]] bool isPlaying() const { return _isPlaying; }
    void play() { _isPlaying = true; }
    void pause() { _isPlaying = false; }
    void stop() {
        _isPlaying = false;
        _currentFrame = 0;
        _timeSinceLastFrame = 0;
    }
    [[nodiscard]] bool isLooping() const { return _loop; }
    void setLooping(bool loop) { _loop = loop; }

  private:
    RenderComponent *_render{nullptr};
    FrameInfo _frameInfo{};
    int _currentFrame{};
    float _timeSinceLastFrame{};
    bool _isPlaying{false};
    bool _loop{false};
};
} // namespace engine
