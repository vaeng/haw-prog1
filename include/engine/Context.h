#pragma once

#include "engine/MessageBus.h"
#include <SFML/Graphics/RenderWindow.hpp>

namespace engine {

struct Context {
    sf::RenderWindow *window{nullptr};
    MessageBus messageBus{};
};
} // namespace engine
