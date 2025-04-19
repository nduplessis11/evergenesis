export module Renderer.Glyph;

export class GlyphRenderer {
public:
    GlyphRenderer() = default;
    ~GlyphRenderer() = default;

    /// Stub initialization (later: load font atlas, setup VBO/VAO shaders)
    static auto init() -> bool;
};