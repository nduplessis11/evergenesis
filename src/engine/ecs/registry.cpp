module;
#include <ranges>

module Engine.Ecs.Registry;

auto Registry::create_entity() -> Entity {
    return entity_manager_.create_entity();
}

void Registry::destroy_entity(const Entity entity) {
    if (!entity_manager_.is_alive(entity)) {
        return;
    }
    // Remove all component data for this entity
    for (const auto& storage : component_storages_ | std::views::values) {
        storage->remove(entity);
    }
    entity_manager_.destroy_entity(entity);
}