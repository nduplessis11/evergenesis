module;
#include <ranges>

module Engine.Ecs.World;

auto EcsWorld::create_entity() -> Entity {
    return entity_manager_.create_entity();
}

void EcsWorld::destroy_entity(const Entity entity) {
    if (!entity_manager_.is_alive(entity)) {
        return;
    }
    // Remove all component data for this entity
    for (const auto& storage : component_storages_ | std::views::values) {
        storage->remove(entity);
    }
    entity_manager_.destroy_entity(entity);
}