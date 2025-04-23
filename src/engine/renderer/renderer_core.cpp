//-----------------------------------------------------------------------------
// renderer_core.cpp
//-----------------------------------------------------------------------------
module;
#include <utility> // for std::move

module Renderer.Core;

RenderSystem::RenderSystem(GraphicsContext& graphics_context,
                           GlyphRenderer&&  glyph_renderer)
    : graphics_context_(graphics_context),
      glyph_renderer_(std::move(glyph_renderer)) {
    // The RenderSystem now *owns* glyph_renderer_
    // but does NOT own graphics_context_.
}

// RenderSystem::~RenderSystem() {
    // 1. Don't call graphics_context_.cleanup() here;
    //    that's the job of whoever *owns* the GraphicsContext.
    // 2. No need to call glyph_renderer_.cleanup() — the GlyphRenderer
    //    destructor calls cleanup() automatically.
// }

// ReSharper disable once CppMemberFunctionMayBeConst
void RenderSystem::update(float /*delta_time*/) {
    // Clear screen to a dark gray
    constexpr Color DARK_GREY_COLOR{.r=0.1F, .g=0.1F, .b=0.1F, .a=1.0F};

    GraphicsContext::begin_frame(DARK_GREY_COLOR);

    // Example usage
    glyph_renderer_.render_text("hello", 1, 1);

    graphics_context_.end_frame();
}
