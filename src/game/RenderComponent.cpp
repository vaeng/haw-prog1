#include "game/RenderComponent.h"
#include "game/GameObject.h"

void RenderComponent::render() { auto transform = owner->getWorldTransform(); }