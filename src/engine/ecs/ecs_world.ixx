module;
#include <memory>
#include <typeindex>
#include <unordered_map>

export module Engine.Ecs.World;

import Engine.Ecs.Entity;
import Engine.Ecs.ComponentStorage;

export class EcsWorld {
public:
    // Create or recycle an entity
    auto create_entity() -> Entity;

    // Destroy an entity and remove its components
    void destroy_entity(Entity entity);

private:
    EntityManager entity_manager_;
    std::unordered_map <std::type_index, std::unique_ptr<IComponentStorage>> component_storages_;
};