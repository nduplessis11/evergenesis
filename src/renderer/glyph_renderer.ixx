module;
#include <array>
#include <cstdint>

export module Renderer.Glyph;

constexpr uint32_t PROJECTION_MATRIX_SIZE = 16;

export class GlyphRenderer {
public:
    GlyphRenderer() = default;
    ~GlyphRenderer() = default;

    /// Stub initialization (later: load font atlas, setup VBO/VAO shaders)
    static auto init(const char* atlas_path, uint32_t screen_width, uint32_t screen_height) -> bool;
    static auto cleanup() -> void;

    /// Render a string at tile coords (x,y) in characters
    static void render_text(const char* glyphs, uint32_t cols, uint32_t rows);
private:
    static uint32_t font_texture_;
    static uint32_t vao_;
    static uint32_t vbo_;
    static int32_t u_projection_loc_;
    static std::array<float, PROJECTION_MATRIX_SIZE> projection_matrix_;
    static uint32_t shader_program_;
    static float glyph_width_;
    static float glyph_height_;
    static uint32_t atlas_cols_;
    static uint32_t atlas_rows_;

    static uint32_t screen_width_;
    static uint32_t screen_height_;
};