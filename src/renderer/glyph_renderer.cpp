module;
#include <print>

module Renderer.Glyph;

bool GlyphRenderer::init() {
    // TODO: load CPU437 font atlas, generate quad buffers, compile shaders, etc.
    std::println("[GlyphRenderer] initialized.");
    return true;
}
