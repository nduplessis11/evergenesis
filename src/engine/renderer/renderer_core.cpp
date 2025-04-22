module;
#include <utility>

module Renderer.Core;

RenderSystem::RenderSystem(GraphicsContext& graphics_context,
                           GlyphRenderer&&  glyph_renderer)
    : graphics_context_(graphics_context),
      glyph_renderer_(std::move(glyph_renderer)) {}

RenderSystem::~RenderSystem() {
    graphics_context_.cleanup();
    glyph_renderer_.cleanup();
}

void RenderSystem::update(float delta_time) const {
    //-------------------------------------------------------------------------
    // Clear Screen
    //-------------------------------------------------------------------------
    constexpr Color DARK_GREY_COLOR{.r = 0.1F, .g = 0.1F, .b = 0.1F, .a = 1.0F};

    graphics_context_.begin_frame(DARK_GREY_COLOR);
    glyph_renderer_.render_text("hello", 1, 1);
    graphics_context_.end_frame();
}