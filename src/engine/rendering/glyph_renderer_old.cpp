//-----------------------------------------------------------------------------
// glyph_renderer.cpp
//-----------------------------------------------------------------------------
module;
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/type_ptr.inl"

#include <SDL3_image/SDL_image.h>
#include <array>
#include <glad/gl.h>
#include <optional>
#include <print>
#include <vector>

module Engine.Rendering.GlyphRenderer;

import Engine.Config.TileConfig;

//-----------------------------------------------------------------------------
// Module-level constants
//-----------------------------------------------------------------------------
static constexpr float    GLYPH_WIDTH       = static_cast<float>(TILE_WIDTH);
static constexpr float    GLYPH_HEIGHT      = static_cast<float>(TILE_HEIGHT);
static constexpr uint32_t ATLAS_COLS        = 32;
static constexpr uint32_t ATLAS_ROWS        = 8;
static constexpr size_t   VERTICES_PER_QUAD = 6;
static constexpr size_t   FLOATS_PER_VERTEX = 4;

//-----------------------------------------------------------------------------
// Internal Helpers
//-----------------------------------------------------------------------------
static auto compile_shader(const uint32_t type, const char* src) -> uint32_t {
    const uint32_t shader_id = glCreateShader(type);
    glShaderSource(shader_id, 1, &src, nullptr);
    glCompileShader(shader_id);
    return shader_id;
}

//-----------------------------------------------------------------------------
// Factory and Special Members
//-----------------------------------------------------------------------------
auto GlyphRenderer::create(const char* atlas_path, const uint32_t screen_width,
        const uint32_t screen_height) -> std::optional<GlyphRenderer> {
    GlyphRenderer instance;
    if (!instance.init(atlas_path, screen_width, screen_height)) {
        return std::nullopt;
    }
    return std::make_optional<GlyphRenderer>(std::move(instance));
}

GlyphRenderer::GlyphRenderer(GlyphRenderer&& other) noexcept
    : font_texture_(other.font_texture_), vao_(other.vao_), vbo_(other.vbo_),
      shader_program_(other.shader_program_),
      u_projection_loc_(other.u_projection_loc_),
      projection_matrix_(other.projection_matrix_),
      glyph_width_(other.glyph_width_), glyph_height_(other.glyph_height_),
      atlas_cols_(other.atlas_cols_), atlas_rows_(other.atlas_rows_),
      screen_width_(other.screen_width_), screen_height_(other.screen_height_) {
    other.font_texture_     = 0;
    other.vao_              = 0;
    other.vbo_              = 0;
    other.shader_program_   = 0;
    other.u_projection_loc_ = -1;
    other.screen_width_     = 0;
    other.screen_height_    = 0;
}

auto GlyphRenderer::operator=(GlyphRenderer&& other) noexcept
        -> GlyphRenderer& {
    if (this != &other) {
        cleanup();
        font_texture_      = other.font_texture_;
        vao_               = other.vao_;
        vbo_               = other.vbo_;
        shader_program_    = other.shader_program_;
        u_projection_loc_  = other.u_projection_loc_;
        projection_matrix_ = other.projection_matrix_;
        glyph_width_       = other.glyph_width_;
        glyph_height_      = other.glyph_height_;
        atlas_cols_        = other.atlas_cols_;
        atlas_rows_        = other.atlas_rows_;
        screen_width_      = other.screen_width_;
        screen_height_     = other.screen_height_;

        other.font_texture_     = 0;
        other.vao_              = 0;
        other.vbo_              = 0;
        other.shader_program_   = 0;
        other.u_projection_loc_ = -1;
        other.screen_width_     = 0;
        other.screen_height_    = 0;
    }
    return *this;
}

GlyphRenderer::~GlyphRenderer() {
    cleanup();
}

//-----------------------------------------------------------------------------
// Initialization & Cleanup
//-----------------------------------------------------------------------------
auto GlyphRenderer::init(const char* atlas_path, const uint32_t screen_width,
        const uint32_t screen_height) -> bool {
    screen_width_  = screen_width;
    screen_height_ = screen_height;
    glyph_width_   = GLYPH_WIDTH;
    glyph_height_  = GLYPH_HEIGHT;
    atlas_cols_    = ATLAS_COLS;
    atlas_rows_    = ATLAS_ROWS;

    SDL_Surface* surface = IMG_Load(atlas_path);
    if (surface == nullptr) {
        std::println(
                stderr, "Failed to load '{}': {}", atlas_path, SDL_GetError());
        return false;
    }

    glGenTextures(1, &font_texture_);
    glBindTexture(GL_TEXTURE_2D, font_texture_);
    const int32_t format =
            (SDL_BYTESPERPIXEL(surface->format) == 4 ? GL_RGBA : GL_RGB);
    glTexImage2D(GL_TEXTURE_2D,
            0,
            format,
            surface->w,
            surface->h,
            0,
            format,
            GL_UNSIGNED_BYTE,
            surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    SDL_DestroySurface(surface);

    constexpr auto VS_SOURCE = R"GLSL(
    #version 330 core
    layout(location=0) in vec4 a_pos_uv;
    out vec2 v_uv;
    uniform mat4 u_proj;
    void main() {
        gl_Position = u_proj * vec4(a_pos_uv.xy, 0, 1);
        v_uv = a_pos_uv.zw;
    }
    )GLSL";

    constexpr auto FS_SOURCE = R"GLSL(
    #version 330 core
    in vec2 v_uv;
    uniform sampler2D u_tex;
    out vec4 frag;
    void main() {
        frag = texture(u_tex, v_uv);
    }
    )GLSL";

    shader_program_            = glCreateProgram();
    const uint32_t vert_shader = compile_shader(GL_VERTEX_SHADER, VS_SOURCE);
    const uint32_t frag_shader = compile_shader(GL_FRAGMENT_SHADER, FS_SOURCE);
    glAttachShader(shader_program_, vert_shader);
    glAttachShader(shader_program_, frag_shader);
    glLinkProgram(shader_program_);
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    u_projection_loc_ = glGetUniformLocation(shader_program_, "u_proj");

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
            VERTICES_PER_QUAD * FLOATS_PER_VERTEX * sizeof(float),
            nullptr,
            GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    const auto width  = static_cast<float>(screen_width_);
    const auto height = static_cast<float>(screen_height_);

    projection_matrix_ = glm::ortho(0.0F, width, height, 0.0F, -1.0F, 1.0F);

    return true;
}

void GlyphRenderer::cleanup() {
    if (shader_program_ != 0U) {
        glDeleteProgram(shader_program_);
        shader_program_ = 0;
    }
    if (vbo_ != 0U) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (vao_ != 0U) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    if (font_texture_ != 0U) {
        glDeleteTextures(1, &font_texture_);
        font_texture_ = 0;
    }
}

//-----------------------------------------------------------------------------
// Rendering: Single-string and Batch
//-----------------------------------------------------------------------------
auto GlyphRenderer::render_text(const char* text, const int32_t start_col,
        const int32_t start_row) const -> void {
    glUseProgram(shader_program_);

    // Bind the VAO/VBO for drawing quads
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    // Activate texture unit 0 and bind the font atlas
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font_texture_);

    // Enable blending for alpha transparency (font antialiasing)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Upload the orthographic projection matrix to the shader
    glUniformMatrix4fv(
            u_projection_loc_, 1, GL_FALSE, glm::value_ptr(projection_matrix_));

    // ------------------------------------------------------------------------
    // Compute how much UV space each glyph occupies
    // ------------------------------------------------------------------------
    // atlas_cols_ --------> number of glyphs per row in the PNG (e.g., 32)
    // atlas_rows_ --------> number of glyph-rows in the PNG (e.g., 8)
    // tile indices -------> atlas (texture) coordinates
    // col_idx / row_idx --> screen (pixel) coordinates
    // uv_scale_x ---------> width (normalized UV) of one glyph cell in texture
    // uv_scale_y ---------> height (normalized UV) of one glyph cell in texture
    //-------------------------------------------------------------------------
    const float    uv_scale_x = 1.F / static_cast<float>(atlas_cols_);
    const float    uv_scale_y = 1.F / static_cast<float>(atlas_rows_);
    uint32_t       col_idx    = start_col; // Current column on screen
    const uint32_t row_idx    = start_row; // Fixed starting row

    // Iterate over each character in the null-terminated string
    for (const char* ptr = text; *ptr != 0; ++ptr, ++col_idx) {
        // Get the character code (0-255)
        const auto glyph_code = static_cast<uint8_t>(*ptr);

        // Determine the glyph's tile position in the atlas grid
        const uint32_t tile_x_idx = glyph_code % atlas_cols_;
        const uint32_t tile_y_idx = glyph_code / atlas_cols_;

        // Calculate UV coordinates in normalized [0,1] space
        const float min_u = static_cast<float>(tile_x_idx) * uv_scale_x;
        const float min_v = static_cast<float>(tile_y_idx) * uv_scale_y;
        const float max_u = min_u + uv_scale_x;
        const float max_v = min_v + uv_scale_y;

        // Calculate the screen position of the glyph in pixels
        const float pos_x = static_cast<float>(col_idx) * glyph_width_;
        const float pos_y = static_cast<float>(row_idx) * glyph_height_;

        // Build the 6 vertices (2 triangles) for the quad:
        // Each vertex has 4 floats: x, y, u, v
        // Triangle 1: top-left, bottom-left, bottom-right
        // Triangle 2: top-left, bottom-right, top-right
        // clang-format off
        std::array<std::array<float, FLOATS_PER_VERTEX>, VERTICES_PER_QUAD> verts = {{
            {{ pos_x,                 pos_y + glyph_height_, min_u, max_v }},
            {{ pos_x,                 pos_y,                 min_u, min_v }},
            {{ pos_x + glyph_width_,  pos_y,                 max_u, min_v }},
            {{ pos_x,                 pos_y + glyph_height_, min_u, max_v }},
            {{ pos_x + glyph_width_,  pos_y,                 max_u, min_v }},
            {{ pos_x + glyph_width_,  pos_y + glyph_height_, max_u, max_v }}
        }};
        // clang-format on

        // Update the VBO with our quad's vertex data
        glBufferSubData(GL_ARRAY_BUFFER,
                0,
                VERTICES_PER_QUAD * FLOATS_PER_VERTEX * sizeof(float),
                verts.data());
        glDrawArrays(GL_TRIANGLES, 0, VERTICES_PER_QUAD);
    }
}

void GlyphRenderer::render_console(
        const char* glyphs, const uint32_t cols, const uint32_t rows) const {
    glUseProgram(shader_program_);

    // Bind the VAO/VBO for drawing batches of quads
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    // Activate and bind the font atlas texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font_texture_);

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Upload the orthographic projection matrix to the shader
    glUniformMatrix4fv(
            u_projection_loc_, 1, GL_FALSE, glm::value_ptr(projection_matrix_));

    // Compute UV step per glyph in atlas
    const float uv_step_x = 1.F / static_cast<float>(atlas_cols_);
    const float uv_step_y = 1.F / static_cast<float>(atlas_rows_);

    // Tile size in pixels
    const float tile_w = glyph_width_;
    const float tile_h = glyph_height_;

    // Pre-allocate the vertex array for the whole console
    std::vector<float> verts;
    verts.reserve(static_cast<size_t>(cols) * rows * VERTICES_PER_QUAD *
                  FLOATS_PER_VERTEX);

    // Loop rows then columns to fill the screen in text grid order
    for (uint32_t row_idx = 0; row_idx < rows; ++row_idx) {
        for (uint32_t col_idx = 0; col_idx < cols; ++col_idx) {
            // Get glyph code from the flat array
            const auto glyph_code =
                    static_cast<uint8_t>(glyphs[(row_idx * cols) + col_idx]);

            // Compute tile indices in atlas
            const uint32_t tile_x_idx = glyph_code % atlas_cols_;
            const uint32_t tile_y_idx = glyph_code / atlas_cols_;

            // Compute UV boundaries for this glyph
            const float min_u = static_cast<float>(tile_x_idx) * uv_step_x;
            const float min_v = static_cast<float>(tile_y_idx) * uv_step_y;
            const float max_u = min_u + uv_step_x;
            const float max_v = min_v + uv_step_y;

            // Compute screen position of the glyph
            const float pos_x = static_cast<float>(col_idx) * tile_w;
            const float pos_y = static_cast<float>(row_idx) * tile_h;

            // Build the 6 vertices (2 triangles) for the quad:
            // clang-format off
            verts.insert(verts.end(),
                         // x,            y,              u,     v
                         {pos_x,      pos_y + tile_h, min_u, max_v,
                          pos_x,          pos_y,          min_u, min_v,
                          pos_x + tile_w, pos_y,          max_u, min_v,
                          pos_x,          pos_y + tile_h, min_u, max_v,
                          pos_x + tile_w, pos_y,          max_u, min_v,
                          pos_x + tile_w, pos_y + tile_h, max_u, max_v});
            // clang-format on
        }
    }

    // Upload entire batch vertex data to GPU
    const auto size = static_cast<GLsizeiptr>(verts.size() * sizeof(float));
    glBufferData(GL_ARRAY_BUFFER, size, verts.data(), GL_DYNAMIC_DRAW);

    // Draw all quads in one call (count = total vertices)
    const auto count = static_cast<GLsizei>(verts.size() / FLOATS_PER_VERTEX);
    glDrawArrays(GL_TRIANGLES, 0, count);
}
