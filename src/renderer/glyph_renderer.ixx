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

    /// Render a string at tile coords (x,y) in characters
    static void render_text(const char* text, int start_x, int start_y);
private:
    static uint32_t font_texture_;
    static uint32_t vao_;
    static uint32_t vbo_;
    static uint32_t shader_program_;
    static uint32_t glyph_width_;
    static uint32_t glyph_height_;
    static uint32_t atlas_cols_;
    static uint32_t atlas_rows_;

    static uint32_t screen_width_;
    static uint32_t screen_height_;
};