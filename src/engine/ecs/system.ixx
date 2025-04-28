export module Engine.Ecs.System;

import Engine.Ecs.Registry;

export struct ISystem {
    virtual ~ISystem() = default;
    virtual void update(Registry& registry) = 0;
};