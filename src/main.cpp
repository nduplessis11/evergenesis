//-----------------------------------------------------------------------------
// main.cpp
//-----------------------------------------------------------------------------
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"

#include <optional>
#include <print>
#include <utility>

// These 'import' statements depend on your build setup and how you're using C++20 modules.
// Adjust if needed.
import Renderer.Core;
import Renderer.Glyph;
import Engine.Core;
import Engine.Ecs.World;
import Engine.Ecs.Entity;
import Game.World.Dungeon;
import Game.Components.DungeonMap;

constexpr int         SCREEN_WIDTH    = 800;
constexpr int         SCREEN_HEIGHT   = 600;
static constexpr auto FONT_ATLAS_PATH = "assets/fonts/cp437_8x16.png";

auto main() -> int
{
    //-------------------------------------------------------------------------
    // INIT DATA
    //-------------------------------------------------------------------------
    Dungeon dungeon({ .width = 80, .height = 25 });
    dungeon.generate();

    EcsWorld world;
    Entity dungeon_entity = world.create_entity();
    world.add_component<DungeonMap>(dungeon_entity, &dungeon);

    //---------------------------------------------------------------------
    // 1) Create the GraphicsContext
    //---------------------------------------------------------------------
    auto maybe_graphics_context = GraphicsContext::create(
        "Sandbox", SCREEN_WIDTH, SCREEN_HEIGHT
    );
    if (!maybe_graphics_context) {
        return -1; // failed to init SDL/window
    }

    // Move the optional's GraphicsContext into a local variable
    GraphicsContext graphics_context = std::move(*maybe_graphics_context);

    //---------------------------------------------------------------------
    // 2) Create the GlyphRenderer
    //---------------------------------------------------------------------
    auto maybe_glyph_renderer = GlyphRenderer::create(
        FONT_ATLAS_PATH, SCREEN_WIDTH, SCREEN_HEIGHT
    );
    if (!maybe_glyph_renderer) {
        // If glyph init fails, we bail. The graphics_context destructor
        // will clean up the window/context automatically.
        return -1;
    }
    GlyphRenderer glyph_renderer = std::move(*maybe_glyph_renderer);

    //---------------------------------------------------------------------
    // 3) Create the RenderSystem (or any other rendering pipeline object)
    //---------------------------------------------------------------------
    // Remove 'const' so we can call 'update()' freely.
    RenderSystem core_renderer(graphics_context, std::move(glyph_renderer));

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
    // If your GraphicsContext destructor does *not* call SDL_Quit(),
    // do it here to clean up SDL for the entire application.
    SDL_Quit();

    return 0;
}
