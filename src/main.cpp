//-----------------------------------------------------------------------------
// main.cpp
//-----------------------------------------------------------------------------
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"

#include <optional>
#include <print>
#include <utility>

import Engine.Core; // GraphicsContext
import Engine.Ecs.Registry; // Registry
import Engine.Ecs.Entity; // Entity
import Engine.Rendering.Systems.Core; // RenderSystem
import Engine.Rendering.RendererInterface; // IRenderer
import Engine.Rendering.OpenGlRenderer; // OpenGLRenderer
import Game.World.Dungeon; // Dungeon
import Game.World.Dungeon.Systems.DungeonToTileMap; // DungeonToTileMapSystem
import Game.Actors.PlayerFactory; // create_player()

constexpr int         SCREEN_WIDTH    = 800;
constexpr int         SCREEN_HEIGHT   = 600;
constexpr int         TILEMAP_COLS    = 80;
constexpr int         TILEMAP_ROWS    = 25;
static constexpr auto FONT_ATLAS_PATH = "assets/fonts/cp437_8x16.png";

auto main() -> int {
    //------------------------------------------------------------------------
    // 1) Generate dungeon & initialize ECS world
    //------------------------------------------------------------------------
    Dungeon dungeon({.width = TILEMAP_COLS, .height = TILEMAP_ROWS});
    dungeon.generate();

    Registry               world;
    DungeonToTileMapSystem map_system(dungeon);
    map_system.initialize(world);

    // spawn player at tile (2,2)
    [[maybe_unused]] Entity player = create_player(world, 2, 2);

    //------------------------------------------------------------------------
    // 2) Create the GraphicsContext (SDL + GL)
    //------------------------------------------------------------------------
    auto maybe_gc =
            GraphicsContext::create("Sandbox", SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!maybe_gc) {
        std::println("Failed to initialize graphics context");
        return -1;
    }
    GraphicsContext graphics_context = std::move(*maybe_gc);

    //------------------------------------------------------------------------
    // 3) Create the OpenGL‐based renderer (implements IRenderer)
    //------------------------------------------------------------------------
    auto renderer = OpenGlRenderer::create(
            FONT_ATLAS_PATH, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!renderer) {
        std::println("Failed to initialize renderer");
        return -1;
    }

    //------------------------------------------------------------------------
    // 4) Hook up the RenderSystem with our ECS world + renderer
    //------------------------------------------------------------------------
    RenderSystem render_system(graphics_context, std::move(renderer));
    render_system.set_world(world);

    //------------------------------------------------------------------------
    // 5) Main loop
    //------------------------------------------------------------------------
    bool is_running = true;
    while (is_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
        }

        constexpr float DELTA_TIME = 1.F / 60.F;
        render_system.update(DELTA_TIME);
    }

    //------------------------------------------------------------------------
    // 6) Cleanup
    //------------------------------------------------------------------------
    SDL_Quit();
    return 0;
}
