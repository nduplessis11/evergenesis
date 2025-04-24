//-----------------------------------------------------------------------------
// glyph_renderer.ixx
//-----------------------------------------------------------------------------
module;
#include "glm/mat4x4.hpp"

#include <cstdint>
#include <optional>

export module Engine.Rendering.GlyphRenderer;

//-----------------------------------------------------------------------------
// Public API
//-----------------------------------------------------------------------------
constexpr uint32_t PROJECTION_MATRIX_SIZE = 16;

// Loads a bitmap font atlas and renders strings or full-screen consoles.
export class GlyphRenderer {
public:
    // Factory: constructs and initializes a renderer, or returns nullopt on
    // failure.
    static auto create(const char* atlas_path, uint32_t screen_width,
                       uint32_t screen_height) -> std::optional<GlyphRenderer>;

    // non-copyable, movable
    GlyphRenderer(const GlyphRenderer&) = delete;
    GlyphRenderer(GlyphRenderer&&) noexcept;
    auto operator=(GlyphRenderer&&) noexcept -> GlyphRenderer&;
    ~GlyphRenderer();

    // Render a null-terminated string at tile coordinates (col, row).
    void render_text(const char* text, int32_t start_col,
                     int32_t start_row) const;

    // Batch-render a full buffer of size cols × rows.
    void render_console(const char* glyphs, uint32_t cols, uint32_t rows) const;

    // Release GPU resources (safe to call multiple times).
    void cleanup();

private:
    // private constructor used by factory
    GlyphRenderer() = default;
    auto init(const char* atlas_path, uint32_t screen_width,
              uint32_t screen_height) -> bool;

    // GPU resources and configuration
    uint32_t  font_texture_{0};
    uint32_t  vao_{0};
    uint32_t  vbo_{0};
    uint32_t  shader_program_{0};
    int32_t   u_projection_loc_{-1};
    glm::mat4 projection_matrix_{};

    float    glyph_width_{};
    float    glyph_height_{};
    uint32_t atlas_cols_{};
    uint32_t atlas_rows_{};
    uint32_t screen_width_{};
    uint32_t screen_height_{};
};
