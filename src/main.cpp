//-----------------------------------------------------------------------------
// main.cpp
//-----------------------------------------------------------------------------
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "glm/mat4x4.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <optional>
#include <print>
#include <utility>

import Engine.Platform.Sdl; // GraphicsContext
import Engine.Ecs.Registry; // Registry
import Engine.Ecs.Entity; // Entity
import Engine.Rendering.RendererInterface; // IRenderer
import Game.World.Dungeon; // Dungeon
import Game.World.Dungeon.Systems.DungeonToTileMap; // DungeonToTileMapSystem
import Game.Actors.PlayerFactory; // create_player()
import Engine.Rendering.Renderer;
import Engine.Renderer.GlBackend;
import Engine.Rendering.Glyph;
import Engine.Config.TileConfig;
import Engine.Core;

constexpr int SCREEN_WIDTH  = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int TILEMAP_COLS  = 80;
constexpr int TILEMAP_ROWS  = 25;

static constexpr auto     FONT_ATLAS_PATH = "assets/fonts/cp437_8x16.png";
static constexpr float    GLYPH_WIDTH     = static_cast<float>(TILE_WIDTH);
static constexpr float    GLYPH_HEIGHT    = static_cast<float>(TILE_HEIGHT);
static constexpr uint32_t ATLAS_COLS      = 32;
static constexpr uint32_t ATLAS_ROWS      = 8;

constexpr Color DARK_GREY_COLOR{.r = 0.1F, .g = 0.1F, .b = 0.1F, .a = 1.0F};

constexpr auto VS_SOURCE = R"GLSL(
    #version 330 core
    layout(location=0) in vec4 a_pos_uv;
    out vec2 v_uv;
    uniform mat4 u_proj;
    void main() {
        gl_Position = u_proj * vec4(a_pos_uv.xy, 0, 1);
        v_uv = a_pos_uv.zw;
    }
    )GLSL";

constexpr auto FS_SOURCE = R"GLSL(
    #version 330 core
    in vec2 v_uv;
    uniform sampler2D u_tex;
    out vec4 frag;
    void main() {
        frag = texture(u_tex, v_uv);
    }
    )GLSL";

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
    auto maybe_gc = SdlGlGraphicsContext::create(
            "Sandbox", SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!maybe_gc) {
        std::println(stderr, "Failed to initialize graphics context");
        return -1;
    }
    SdlGlGraphicsContext graphics_context = std::move(*maybe_gc);

    //------------------------------------------------------------------------
    // 3) Create the OpenGL‐based renderer (implements IRenderer)
    //------------------------------------------------------------------------
    // auto renderer = OpenGlRenderer::create(
    //         FONT_ATLAS_PATH, SCREEN_WIDTH, SCREEN_HEIGHT);
    // if (!renderer) {
    //     std::println("Failed to initialize renderer");
    //     return -1;
    // }

    Renderer::RendererFrontend frontend;
    auto backend = std::make_unique<Renderer::GlBackend>();
    backend->initialize();

    backend->load_shader("glyph_shader", VS_SOURCE, FS_SOURCE);
    glm::mat4 proj =
            glm::ortho(0.F, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 0.F, -1.F, 1.F);
    backend->set_projection_matrix(proj);

    auto                    img = load_image(FONT_ATLAS_PATH);
    Renderer::TextureHandle font_texture;
    Renderer::GlBackend::load_texture(font_texture, img);

    GlyphResource     glyph_resource{font_texture,
                                 "glyph_shader",
                                 ATLAS_COLS,
                                 ATLAS_ROWS,
                                 GLYPH_WIDTH,
                                 GLYPH_HEIGHT};
    GlyphRenderSystem glyph_sys(frontend, glyph_resource);

    //------------------------------------------------------------------------
    // 4) Hook up the RenderSystem with our ECS world + renderer
    //------------------------------------------------------------------------
    // RenderSystem render_system(graphics_context, std::move(renderer));
    // render_system.set_world(world);

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

        // constexpr float DELTA_TIME = 1.F / 60.F;
        // render_system.update(DELTA_TIME);

        // Begin frame: clear via platform layer
        SdlGlGraphicsContext::begin_frame({DARK_GREY_COLOR});

        // Update render system (populates frontend)
        glyph_sys.update(world);

        // Execute one draw call for all glyphs
        backend->execute(frontend.get_commands());

        // Present frame
        graphics_context.end_frame();

        // Reset command queue
        frontend.clear();
    }

    //------------------------------------------------------------------------
    // 6) Cleanup
    //------------------------------------------------------------------------
    SDL_Quit();
    return 0;
}
