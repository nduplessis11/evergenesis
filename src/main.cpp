//-----------------------------------------------------------------------------
// main.cpp
//-----------------------------------------------------------------------------
#include "SDL3/SDL_events.h"
#include "glad/gl.h"
#include <optional>
#include <print>
#include <utility>

import Renderer.Core;
import Renderer.Glyph;

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------
constexpr int         SCREEN_WIDTH    = 800;
constexpr int         SCREEN_HEIGHT   = 600;
static constexpr auto FONT_ATLAS_PATH = "assets/fonts/cp437_8x16.png";

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------
struct Color {
    float r;
    float g;
    float b;
    float a;
};

//-----------------------------------------------------------------------------
// Entry Point
//-----------------------------------------------------------------------------
auto main() -> int {
    //-------------------------------------------------------------------------
    // Core Renderer Initialization
    //-------------------------------------------------------------------------
    RenderSystem core_renderer;
    if (!core_renderer.init("Colony Sim",
                            static_cast<uint32_t>(SCREEN_WIDTH),
                            static_cast<uint32_t>(SCREEN_HEIGHT))) {
        return -1;
    }

    //-------------------------------------------------------------------------
    // Glyph Renderer Setup
    //-------------------------------------------------------------------------
    // ReSharper disable once CppLocalVariableMayBeConst
    auto maybe_glyph_renderer = GlyphRenderer::create(
                             FONT_ATLAS_PATH,
                             SCREEN_WIDTH,
                             SCREEN_HEIGHT);
    if (!maybe_glyph_renderer) {
        core_renderer.cleanup();
        return -1;
    }
    GlyphRenderer glyph_renderer = std::move(*maybe_glyph_renderer);

    //-------------------------------------------------------------------------
    // Main Loop
    //-------------------------------------------------------------------------
    bool is_running = true;
    while (is_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                is_running = false;
            }
        }

        //---------------------------------------------------------------------
        // Clear Screen
        //---------------------------------------------------------------------
        constexpr Color DARK_GREY_COLOR{
                                 .r = 0.1F, .g = 0.1F, .b = 0.1F, .a = 1.0F};
        glClearColor(DARK_GREY_COLOR.r,
                     DARK_GREY_COLOR.g,
                     DARK_GREY_COLOR.b,
                     DARK_GREY_COLOR.a);
        glClear(GL_COLOR_BUFFER_BIT);

        //---------------------------------------------------------------------
        // Render Text Overlay
        //---------------------------------------------------------------------
        glyph_renderer.render_text("hello", 1, 1);

        //---------------------------------------------------------------------
        // Swap Buffers
        //---------------------------------------------------------------------
        core_renderer.swap_buffers();
    }

    //-------------------------------------------------------------------------
    // Cleanup
    //-------------------------------------------------------------------------
    glyph_renderer.cleanup();
    core_renderer.cleanup();
    return 0;
}