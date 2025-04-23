// ----------------------------------------------------------------------------
// engine/ecs/component_storage.ixx
// Type-erased, contiguous storage for components of a given type
// ----------------------------------------------------------------------------
module;
#include <cassert>
#include <vector>

export module Engine.Ecs.ComponentStorage;

import Engine.Ecs.Entity;

export struct IComponentStorage {
    virtual ~IComponentStorage()       = default;
    virtual void remove(Entity entity) = 0;
};

export template <typename Component>
class ComponentStorage final : public IComponentStorage {
public:
    void insert(const Entity entity, Component component) {
        if (entity.index >= entity_to_index_.size()) {
            entity_to_index_.resize(entity.index + 1, -1);
        }

        assert(std::cmp_equal(entity_to_index_[entity.index], -1));
        components_.push_back(std::move(component));
        entities_.push_back(entity);
        entity_to_index_[entity.index] = components_.size() - 1;
    }

    auto get(const Entity entity) -> Component* {
        if (entity.index >= entity_to_index_.size()) {
            return nullptr;
        }
        auto idx = entity_to_index_[entity.index];
        return idx < 0 ? nullptr : &components_[idx];
    }

    void remove(const Entity entity) override {
        // 1) If we’ve never resized out to this entity’s index, nothing to do
        if (entity.index >= entity_to_index_.size()) {
            return;
        }

        // 2) Look up where its component lives in the dense array
        auto idx = entity_to_index_[entity.index];
        //    A negative value means “no component,” so bail
        if (idx < 0) {
            return;
        }

        // 3) Compute the index of the *last* element in our dense arrays
        auto last = components_.size() - 1;

        // 4) Overwrite the slot at idx by moving in the last component
        components_[idx] = std::move(components_[last]);
        entities_[idx]   = entities_[last];

        // 5) Update the sparse map entry for that moved entity
        //    (it used to point at 'last', now it points at 'idx')
        entity_to_index_[entities_[idx].index] = idx;

        // 6) Shrink the dense arrays by removing the now‑moved last slot
        components_.pop_back();
        entities_.pop_back();

        // 7) Finally, mark the removed entity’s slot as empty
        entity_to_index_[entity.index] = -1;
    }

private:
    std::vector<Component> components_;
    std::vector<Entity>    entities_;
    std::vector<int32_t>   entity_to_index_;
};