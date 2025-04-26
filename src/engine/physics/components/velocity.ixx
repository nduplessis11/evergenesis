module;
#include "glm/vec2.hpp"

export module Engine.Physics.Components.Velocity;

export struct Velocity {
    glm::vec2 velocity;
    float speed;
};
