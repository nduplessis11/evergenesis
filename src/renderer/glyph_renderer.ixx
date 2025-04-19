module;
#include <cstdint>

export module Renderer.Glyph;

export class GlyphRenderer {
public:
    GlyphRenderer() = default;
    ~GlyphRenderer() = default;

    /// Stub initialization (later: load font atlas, setup VBO/VAO shaders)
    static auto init(const char* atlas_path) -> bool;
    static auto cleanup() -> void;

private:
    static uint32_t font_texture_;
};