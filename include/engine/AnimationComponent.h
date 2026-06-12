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
    AnimationComponent(const FrameInfo &info) : frameInfo(info) {}
    void start() override {
        render = owner->getComponent<RenderComponent>();
        if (render == nullptr) {
            throw std::runtime_error("AnimationComponent requires a RenderComponent");
        }
    }

    void update(float dt) override {
        if (render == nullptr) {
            return;
        }
        timeSinceLastFrame += dt;

        if (timeSinceLastFrame >= 1.0f / frameInfo.framesPerSecond) {
            currentFrame = (currentFrame + 1) % (frameInfo.totalFrames);
            timeSinceLastFrame = 0;
        }
        int left = (currentFrame % frameInfo.horizontalFrameCount) *
                       (frameInfo.width + frameInfo.horizontalPadding) +
                   frameInfo.horizontalOffset;
        int top = (currentFrame / frameInfo.horizontalFrameCount) *
                      (frameInfo.height + frameInfo.verticalPadding) +
                  frameInfo.verticalOffset;
        render->setTextureRect(
            {.left = left, .top = top, .width = frameInfo.width, .height = frameInfo.height});
    }

    [[nodiscard]] auto getFrameInfo() const -> FrameInfo { return frameInfo; }
    auto setFrameInfo(const FrameInfo &info) { frameInfo = info; }

  private:
    RenderComponent *render{nullptr};
    FrameInfo frameInfo{};
    int currentFrame{};
    float timeSinceLastFrame{};
};
} // namespace engine
