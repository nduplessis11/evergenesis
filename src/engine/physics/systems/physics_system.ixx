export module Engine.Physics.Systems.Core;

import Engine.Ecs.World;

export class PhysicsSystem {
    void update(float delta_time);
    void set_world(EcsWorld& world);
};