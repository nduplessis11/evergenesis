module;
#include <memory>
#include <utility>

module Engine.Rendering.OpenGlRenderer;

auto OpenGlRenderer::create(const char* atlas_path, std::uint32_t screen_width,
        std::uint32_t screen_height) -> std::unique_ptr<IRenderer> {
    // Use GlyphRenderer factory to initialize OpenGL glyph rendering
    auto maybe_glyph =
            GlyphRenderer::create(atlas_path, screen_width, screen_height);
    if (!maybe_glyph) {
        return nullptr; // initialization failed (e.g., texture not found)
    }
    // Construct OpenGLRenderer with the initialized GlyphRenderer
    return std::unique_ptr<IRenderer>(
            new OpenGlRenderer(std::move(*maybe_glyph)));
}

OpenGlRenderer::OpenGlRenderer(GlyphRenderer&& glyph_renderer)
    : glyph_renderer_(std::move(glyph_renderer)) {
    // GlyphRenderer is already initialized via its create function
}

void OpenGlRenderer::render_text(const char* text, std::int32_t start_col,
        std::int32_t start_row) const {
    glyph_renderer_.render_text(text, start_col, start_row);
}

void OpenGlRenderer::render_console(
        const char* glyphs, std::uint32_t cols, std::uint32_t rows) const {
    glyph_renderer_.render_console(glyphs, cols, rows);
}
