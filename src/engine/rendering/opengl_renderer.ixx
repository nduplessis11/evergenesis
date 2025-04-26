module;
#include <cstdint>
#include <memory>

export module Engine.Rendering.OpenGlRenderer;

import Engine.Rendering.RendererInterface;
import Engine.Rendering.GlyphRenderer;

export class OpenGlRenderer final : public IRenderer {
public:
    // Create an OpenGL-based renderer (returns nullptr on failure).
    static auto create(const char* atlas_path, std::uint32_t screen_width, std::uint32_t screen_height)
           -> std::unique_ptr<IRenderer>;
    OpenGlRenderer(const OpenGlRenderer&) = delete;
    auto operator=(const OpenGlRenderer&) -> OpenGlRenderer& = delete;
    ~OpenGlRenderer() override                               = default;

    void render_text(const char* text, std::int32_t start_col, std::int32_t start_row) const override;
    void render_console(const char* glyphs, std::uint32_t cols, std::uint32_t rows) const override;
private:
    // Private constructor used by the factory
    explicit OpenGlRenderer(GlyphRenderer&& glyph_renderer);
    GlyphRenderer glyph_renderer_;
};
