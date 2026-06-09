#pragma once
#include "Component.h"
#include "GameObject.h"
#include "RenderComponent.h"

#include <iostream>

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
    void start() override {
        render = owner->getComponent<RenderComponent>();
        if (render == nullptr) {
            throw std::runtime_error("AnimationComponent requires a RenderComponent");
        }
        std::cout << "Starting AnimationComponent \n";
    }

    void update(float dt) override {
        if (render == nullptr) {
            return;
        }
        timeSinceLastFrame += dt;
        auto frameInfo = FrameInfo{.framesPerSecond = 20.F,
                                   .totalFrames = 43,
                                   .left = 0,
                                   .top = 0,
                                   .width = 498,
                                   .height = 374,
                                   .verticalOffset = 2,
                                   .horizontalOffset = 2,
                                   .horizontalFrameCount = 5,
                                   .verticalFrameCount = 9,
                                   .horizontalPadding = 4,
                                   .verticalPadding = 4};

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

  private:
    RenderComponent *render{nullptr};
    int currentFrame{};
    float timeSinceLastFrame{};
};
