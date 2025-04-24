//-----------------------------------------------------------------------------
// renderer_core.cpp
//-----------------------------------------------------------------------------
module;
#include <utility> // for std::move

module Engine.Rendering.Systems.Core;

RenderSystem::RenderSystem(GraphicsContext& graphics_context,
                           GlyphRenderer&&  glyph_renderer)
    : graphics_context_(graphics_context),
      glyph_renderer_(std::move(glyph_renderer)),
      tile_map_renderer_(glyph_renderer) {
    // The RenderSystem now *owns* glyph_renderer_
    // but does NOT own graphics_context_.
}

void RenderSystem::set_world(EcsWorld& world) {
    world_ = &world;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void RenderSystem::update(float /*delta_time*/) {
    // Clear screen to a dark gray
    constexpr Color DARK_GREY_COLOR{.r=0.1F, .g=0.1F, .b=0.1F, .a=1.0F};

    GraphicsContext::begin_frame(DARK_GREY_COLOR);

    // Example usage
    if (world_ != nullptr) {
        tile_map_renderer_.update(*world_);
    } else {
        glyph_renderer_.render_text("hello", 1, 1);
    }

    graphics_context_.end_frame();
}
