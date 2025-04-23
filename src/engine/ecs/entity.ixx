module;
#include <cstdint>
#include <vector>

export module Engine.Ecs.Entity;

export struct Entity {
    uint32_t index;
    uint32_t generation;
};

export class EntityManager {
public:
    EntityManager() = default;
    auto create_entity() -> Entity;
    auto destroy_entity(Entity entity) -> void;
    [[nodiscard]] auto is_alive(const Entity& entity) const -> bool;

private:
    uint32_t next_index_ {0};
    std::vector<uint32_t> generations_;
    std::vector<uint32_t> free_indices_;
};