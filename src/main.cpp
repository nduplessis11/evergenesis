//-----------------------------------------------------------------------------
// main.cpp
//-----------------------------------------------------------------------------
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"

#include <optional>
#include <print>
#include <utility>

import Engine.Rendering.Systems.Core; // RenderSystem
import Engine.Rendering.GlyphRenderer; // GlyphRenderer
import Engine.Core; // GraphicsContext
import Engine.Ecs.World; // EcsWorld
import Engine.Ecs.Entity; // Entity
import Game.World.Dungeon; // Dungeon class
import Game.World.Dungeon.Systems.DungeonToTileMap;
import Game.Actors.PlayerFactory;

constexpr int         SCREEN_WIDTH    = 800;
constexpr int         SCREEN_HEIGHT   = 600;
static constexpr auto FONT_ATLAS_PATH = "assets/fonts/cp437_8x16.png";

auto main() -> int {
    //-------------------------------------------------------------------------
    // INIT DATA
    //-------------------------------------------------------------------------
    Dungeon dungeon({.width = 80, .height = 25});
    dungeon.generate();

    EcsWorld               world;
    DungeonToTileMapSystem map_system(dungeon);
    map_system.initialize(world);

    // spawn player at tile (2,2)
    Entity player = create_player(world, 2, 2);

    //---------------------------------------------------------------------
    // 1) Create the GraphicsContext
    //---------------------------------------------------------------------
    auto maybe_graphics_context = GraphicsContext::create(
                             "Sandbox", SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!maybe_graphics_context) {
        return -1; // failed to init SDL/window
    }

    GraphicsContext graphics_context = std::move(*maybe_graphics_context);

    //---------------------------------------------------------------------
    // 2) Create the GlyphRenderer
    //---------------------------------------------------------------------
    auto maybe_glyph_renderer = GlyphRenderer::create(
                             FONT_ATLAS_PATH, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!maybe_glyph_renderer) {
        return -1;
    }
    GlyphRenderer glyph_renderer = std::move(*maybe_glyph_renderer);

    //---------------------------------------------------------------------
    // 3) Create and hook up the ECS-powered RenderSystem
    //---------------------------------------------------------------------
    RenderSystem core_renderer(graphics_context, std::move(glyph_renderer));
    core_renderer.set_world(world);

    //---------------------------------------------------------------------
    // 4) Main Game / App Loop
    //---------------------------------------------------------------------
    bool is_running = true;
    while (is_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
        }

        constexpr float DELTA_TIME = 0.0167f; // ~60 FPS
        core_renderer.update(DELTA_TIME);
    }

    //---------------------------------------------------------------------
    // 5) Cleanup
    //---------------------------------------------------------------------
    SDL_Quit();
    return 0;
}
