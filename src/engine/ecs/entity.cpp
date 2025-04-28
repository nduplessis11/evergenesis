module;
#include <cstdint>

module Engine.Ecs.Entity;

auto EntityManager::create_entity() -> Entity {
    uint32_t index;
    if (!free_indices_.empty()) {
        index = free_indices_.back();
        free_indices_.pop_back();
        ++generations_[index];
    } else {
        index = next_index_++;
        generations_.push_back(0);
    }
    return Entity{.index = index, .generation = generations_[index]};
}
void EntityManager::destroy_entity(const Entity entity) {
    if (!is_alive(entity)) {
        return;
    }
    free_indices_.push_back(entity.index);
    ++generations_[entity.index];
}

auto EntityManager::is_alive(const Entity& entity) const -> bool {
    return entity.index < generations_.size() &&
           generations_[entity.index] == entity.generation;
}