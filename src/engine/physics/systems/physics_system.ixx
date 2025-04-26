export module Engine.Physics.Systems.Core;

import Engine.Ecs.Registry;

export class PhysicsSystem {
    void update(float delta_time);
    void set_world(Registry& world);
};