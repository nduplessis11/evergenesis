#include "SDL3/SDL_events.h"
#include "glad/gl.h"

#include <print>

import Renderer.Core;
import Renderer.Glyph;

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

static constexpr auto FONT_ATLAS = "assets/fonts/cp437_8x16.png";

auto main() -> int {
    RenderSystem renderer;
    if (!renderer.init("Colony Sim", WIDTH, HEIGHT)) {
        return -1;
    }

    if (GlyphRenderer glyph_renderer; !GlyphRenderer::init(FONT_ATLAS)) {
        renderer.cleanup();
        return -1;
    }

    // Basic event loop stub
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                running = false;
            default:;
            }
        }

        // Clear screen (dark grey)
        glClearColor(0.1F, 0.1F, 0.1F, 1.0F); // NOLINT(*-magic-numbers)
        glClear(GL_COLOR_BUFFER_BIT);

        GlyphRenderer::render_text("hello", 1, 1);

        renderer.swap_buffers();
    }

    GlyphRenderer::cleanup();
    renderer.cleanup();
    return 0;
}