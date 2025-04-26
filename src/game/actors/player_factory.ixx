//-----------------------------------------------------------------------------
// src/game/actors/player_factory.ixx
//-----------------------------------------------------------------------------
module; // global module fragment for includes
#include <glm/vec2.hpp>

export module Game.Actors.PlayerFactory;

import Engine.Ecs.Registry; // EcsWorld
import Engine.Ecs.Entity; // Entity

import Engine.Physics.Components.Transform; // Transform(glm::vec2)
import Engine.Physics.Components.Velocity; // Velocity(glm::vec2, float speed)
import Engine.Physics.Components.Collider; // Collider(float width, float height)

import Engine.Rendering.Components.GlyphRenderable; // GlyphRenderable(char)

import Engine.Config.TileConfig; // TILE_WIDTH, TILE_HEIGHT

export auto create_player(Registry& world, const int tile_x, const int tile_y)
                         -> Entity {
    const float start_x = static_cast<float>(tile_x) * TILE_WIDTH;
    const float start_y = static_cast<float>(tile_y) * TILE_HEIGHT;

    const Entity entity = world.create_entity();

    // Transform takes a glm::vec2
    world.add_component<Transform>(entity, glm::vec2{start_x, start_y});

    // Velocity takes a glm::vec2 and a speed float
    world.add_component<Velocity>(entity,
                                  glm::vec2{0.F, 0.F},
                                  /* speed */ 1.F);

    // Collider takes width and height as separate floats
    world.add_component<Collider>(entity,
                                  static_cast<float>(TILE_WIDTH),
                                  static_cast<float>(TILE_HEIGHT));

    // GlyphRenderable takes a single char
    world.add_component<GlyphRenderable>(entity, '@');

    return entity;
}