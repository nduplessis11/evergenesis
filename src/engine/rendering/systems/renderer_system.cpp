//-----------------------------------------------------------------------------
// renderer_core.cpp
//-----------------------------------------------------------------------------
module;
#include <array>
#include <utility> // for std::move

module Engine.Rendering.Systems.Core;

import Engine.Ecs.World;
import Engine.Physics.Components.Transform;
import Engine.Rendering.Components.GlyphRenderable;
import Engine.Rendering.GlyphRenderer;
import Engine.Config.TileConfig;

RenderSystem::RenderSystem(
        GraphicsContext& graphics_context, GlyphRenderer&& glyph_renderer)
    : graphics_context_(graphics_context),
      glyph_renderer_(std::move(glyph_renderer)),
      tile_map_renderer_(glyph_renderer_) {}

void RenderSystem::set_world(EcsWorld& world) {
    world_ = &world;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void RenderSystem::update(float /*delta_time*/) {
    // Clear screen to a dark gray
    constexpr Color DARK_GREY_COLOR{.r = 0.1F, .g = 0.1F, .b = 0.1F, .a = 1.0F};

    GraphicsContext::begin_frame(DARK_GREY_COLOR);

    // Example usage
    if (world_ != nullptr) {
        tile_map_renderer_.update(*world_);

        // Render all the entities that have a Transform + GlyphRenderable
        world_->for_each<Transform, GlyphRenderable>(
                [&](const Transform&           transform,
                        const GlyphRenderable& glyph_renderable) {
                    // Compute tile coords from the world (pixel) position
                    const int col = static_cast<int32_t>(
                            transform.position.x / TILE_WIDTH);
                    const int row = static_cast<int32_t>(
                            transform.position.y / TILE_HEIGHT);

                    // Render a single-character string at (col, row)
                    const std::array text = {glyph_renderable.glyph, '\0'};
                    glyph_renderer_.render_text(text.data(), col, row);
                });

    } else {
        // Fallback debug text
        glyph_renderer_.render_text(
                "Something went wrong with the world!", 1, 1);
    }

    graphics_context_.end_frame();
}
