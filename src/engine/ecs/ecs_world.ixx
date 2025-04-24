module;
#include <cassert>
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

    // ======= Templated APIs (definitions below) =======

    // Attach a component of type C to an entity
    template <typename C, typename... Args>
    auto add_component(Entity entity, Args&&... args) -> C&;

    // Detach a component of type C from an entity
    template <typename C> auto remove_component(Entity entity) -> void;

    // Check if an entity has a component of type C
    template <typename C>
    [[nodiscard]] auto has_component(Entity entity) const -> bool;

    // Retrieve a reference to an entity's component of type C
    template <typename C> auto get_component(Entity entity) -> C&;

    // Run func(comp1, comp2) for each entity that has both C1 and C2
    template <typename C1, typename C2, typename Func>
    auto for_each(Func func) -> void;

    // Return all entities that currently have a component of type C
    template <typename C>
    [[nodiscard]] auto entities_with() const -> std::vector<Entity>;

private:
    // Helper: get the storage component for type C, or nullptr
    template <typename C> auto get_storage() -> ComponentStorage<C>*;

    EntityManager entity_manager_;
    std::unordered_map<std::type_index, std::unique_ptr<IComponentStorage>>
                             component_storages_;
};

//------------------------------------------------------------------------------
// Definitions of templated methods (must appear in the .ixx interface)
//------------------------------------------------------------------------------
template <typename C, typename... Args>
// C: the component type to attach (e.g., Position, Velocity, Health, etc.)
// Args&&... args: forwarding references — can bind to both lvalues and rvalues
//   * If you pass an lvalue, Args deduces to U& and the parameter type becomes
//   U& &&,
//     which collapses to U& (an lvalue reference).
//   * If you pass an rvalue, Args deduces to U, and the parameter type is U&&.
//   Perfect forwarding via std::forward preserves the original value category.
auto EcsWorld::add_component(Entity entity, Args&&... args) -> C& {
    // 1) Sanity check: entity must be alive. In a debug build this aborts
    //    if it’s not, helping catch errors early.
    assert(entity_manager_.is_alive(entity) && "Entity must be alive");

    // 2) Compute a runtime key for this component type:
    //    typeid(C) gives a std::type_info for C,
    //    std::type_index makes it hashable/comparable for use as a map key.
    const auto type_id = std::type_index(typeid(C));

    // 3) Lazy‐initialize storage for this component type if needed:
    //    component_storages_ maps type_index → unique_ptr<IComponentStorage>.
    if (!component_storages_.contains(type_id)) {
        component_storages_[type_id] = std::make_unique<ComponentStorage<C>>();
    }

    // 4) Retrieve the type‐erased pointer and cast back to the concrete
    // storage:
    //    We just created a ComponentStorage<C>, so this cast is safe.
    auto* storage = static_cast<ComponentStorage<C>*>(
                             component_storages_[type_id].get());

    // 5) Insert the component instance in‐place, forwarding constructor args:
    //    std::forward<Args>(args)... expands to either an lvalue or rvalue
    //    reference cast, depending on how each arg was passed in.
    //    Under the hood, std::forward<T>(x) is roughly:
    //       static_cast<T&&>(x);
    //    and the “reference collapse” rules make sure:
    //      - if T = U& then T&& → U&  (lvalue)
    //      - if T = U  then T&& → U&& (rvalue)
    storage->insert(entity, C{std::forward<Args>(args)...});

    // 6) Return a reference to the newly‐inserted component, so the caller
    //    can immediately read or modify it.
    return *storage->get(entity);
}

template <typename C> void EcsWorld::remove_component(Entity entity) {
    const auto type_id = std::type_index(typeid(C));
    if (const auto iter = component_storages_.find(type_id);
        iter != component_storages_.end()) {
        static_cast<ComponentStorage<C>*>(iter->second.get())->remove(entity);
    }
}

template <typename C>
auto EcsWorld::has_component(Entity entity) const -> bool {
    // 1) Compute the runtime key for this component type once
    const auto type_id = std::type_index(typeid(C));

    // 2) Use find() rather than contains() because:
    //    - contains() only tells us if the key exists (yes/no), but we then
    //      would still need to retrieve the iterator or value to check the
    //      entity.
    //    - find() gives us the iterator directly, so we avoid a second lookup.
    //    This keeps it to a single hash/map probe instead of two.
    if (const auto iter = component_storages_.find(type_id);
        iter != component_storages_.end()) {
        // 3) Cast back to the concrete storage type so we can call get(entity)
        auto* storage = static_cast<ComponentStorage<C>*>(iter->second.get());

        // 4) Return true only if this specific entity has a C component
        return storage->get(entity) != nullptr;
    }

    // 5) No storage for this component type means no entity has it
    return false;
}

template <typename C> auto EcsWorld::get_component(const Entity entity) -> C& {
    assert(has_component<C>(entity) && "Missing component");
    auto* storage = static_cast<ComponentStorage<C>*>(
                             component_storages_.at(std::type_index(typeid(C)))
                                                      .get());
    return *storage->get(entity);
}

template <typename C1, typename C2, typename Func>
auto EcsWorld::for_each(Func func) -> void {
    // 1) Fetch the two component storages; get_storage returns nullptr if
    //    that component type has never been added.
    auto* storage1 = get_storage<C1>();
    auto* storage2 = get_storage<C2>();

    // 2) If either storage is missing, no entity can have both components →
    // exit.
    if (!storage1 || !storage2) {
        return;
    }

    // 3) Get the lists of entities that currently have each component.
    //    These are the “dense” arrays we maintained in ComponentStorage.
    const auto& list1 = storage1->entities_with_component();
    const auto& list2 = storage2->entities_with_component();

    // 4) For efficiency, iterate over the *smaller* list and check membership
    //    in the larger one. This minimizes the total number of lookups.
    if (list1.size() <= list2.size()) {
        // 4a) list1 is smaller (or equal).
        for (auto entity : list1) {
            // 5a) Check if this entity also has C2
            if (storage2->get(entity)) {
                // 6a) Both components exist: retrieve them and invoke the
                // callback
                func(*storage1->get(entity), // C1&
                     *storage2->get(entity) // C2&
                );
            }
        }
    } else {
        // 4b) list2 is smaller.
        for (auto entity : list2) {
            // 5b) Check if this entity also has C1
            if (storage1->get(entity)) {
                // 6b) Both components exist: retrieve them and invoke the
                // callback
                func(*storage1->get(entity), // C1&
                     *storage2->get(entity) // C2&
                );
            }
        }
    }
}

template <typename C>
auto EcsWorld::entities_with() const -> std::vector<Entity> {
    const auto iter = component_storages_.find(std::type_index(typeid(C)));
    if (iter == component_storages_.end()) {
        return {}; // No storage for this component type
    }

    const auto* storage = static_cast<ComponentStorage<C>*>(iter->second.get());
    return storage->entities_with_component();
}

template <typename C> auto EcsWorld::get_storage() -> ComponentStorage<C>* {
    const auto iter = component_storages_.find(std::type_index(typeid(C)));
    return iter == component_storages_.end()
                                    ? nullptr
                                    : static_cast<ComponentStorage<C>*>(
                                                               iter->second.get());
}