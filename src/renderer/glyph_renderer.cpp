module;
#include <array>
#include <vector>
#include <print>
#include <SDL3_image/SDL_image.h>
#include <glad/gl.h>

module Renderer.Glyph;

constexpr uint32_t GLYPH_WIDTH = 8.F;
constexpr uint32_t GLYPH_HEIGHT = 16.F;
constexpr uint32_t ATLAS_COLS = 32;
constexpr uint32_t ATLAS_ROWS = 8;

constexpr size_t VERTICES_PER_QUAD = 6;
constexpr size_t FLOATS_PER_VERTEX = 4;

uint32_t GlyphRenderer::font_texture_ = 0;
uint32_t GlyphRenderer::vao_ = 0;
uint32_t GlyphRenderer::vbo_ = 0;
int32_t GlyphRenderer::u_projection_loc_ = 0;
std::array<float, PROJECTION_MATRIX_SIZE> GlyphRenderer::projection_matrix_;
uint32_t GlyphRenderer::shader_program_ = 0;
float GlyphRenderer::glyph_width_ = GLYPH_WIDTH;
float GlyphRenderer::glyph_height_ = GLYPH_HEIGHT;
uint32_t GlyphRenderer::atlas_cols_ = ATLAS_COLS;
uint32_t GlyphRenderer::atlas_rows_ = ATLAS_ROWS;
uint32_t GlyphRenderer::screen_width_ = 0;
uint32_t GlyphRenderer::screen_height_ = 0;

auto compile_shader(const uint32_t shader_type, const char* src) -> uint32_t {
    const uint32_t shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    return shader;
}

auto GlyphRenderer::init(const char* atlas_path, const uint32_t screen_width,
                         const uint32_t screen_height) -> bool {
    screen_width_ = screen_width;
    screen_height_ = screen_height;

    // Initialize SDL_image of PNG support
    SDL_Surface* surface = IMG_Load(atlas_path);
    if (surface == nullptr) {
        std::println(stderr, "Failed to load font atlas '{}': {}", atlas_path,
                     SDL_GetError());
        return false;
    }

    std::println("[GlyphRenderer] loaded font atlas '{}', texture id {}.",
                 atlas_path, font_texture_);

    glGenTextures(1, &font_texture_);
    glBindTexture(GL_TEXTURE_2D, font_texture_);

    // Determine format
    const int32_t format =
        (SDL_BYTESPERPIXEL(surface->format) == 4 ? GL_RGBA : GL_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format,
                 GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    SDL_DestroySurface(surface);

    // Simple shader
    const auto* const vertex_src = R"GLSL(
    #version 330 core
    layout (location = 0) in vec4 a_pos_uv;

    out vec2 v_uv;
    uniform mat4 u_proj;

    void main() {
        gl_Position = u_proj * vec4(a_pos_uv.xy,0,1);
        v_uv = a_pos_uv.zw;
    }
    )GLSL";

    const auto* const fragment_src = R"GLSL(
    #version 330 core

    in vec2 v_uv;
    uniform sampler2D u_tex;
    out vec4 frag;

    void main() {
        frag = texture(u_tex, v_uv);
    }
    )GLSL";

    const uint32_t program = glCreateProgram();
    const uint32_t vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_src);
    const uint32_t fragment_shader =
        compile_shader(GL_FRAGMENT_SHADER, fragment_src);
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    shader_program_ = program;
    u_projection_loc_ = glGetUniformLocation(shader_program_, "u_proj");

    // VAO/VBO for quad (x,y,u,v)
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * VERTICES_PER_QUAD * FLOATS_PER_VERTEX, nullptr,
                 GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    const float width = static_cast<float>(screen_width_);
    const float height = static_cast<float>(screen_height_);
    // Orthographic projection: assume unit = pixel
    // clang-format off
    projection_matrix_ = {
        2.F/width,  0.F,         0.F, -1.F,     // NOLINT(*-magic-numbers)
        0.F,       -2.F/height,  0.F,  1.F,     // NOLINT(*-magic-numbers)
        0.F,        0.F,        -1.F,  0.F,
        0.F,        0.F,         0.F,  1.F
    };
    // clang-format on

    return true;
}
auto GlyphRenderer::cleanup() -> void {
    if (shader_program_ != 0U) {
        glDeleteProgram(shader_program_);
    }
    if (vbo_ != 0U) {
        glDeleteBuffers(1, &vbo_);
    }
    if (vao_ != 0U) {
        glDeleteVertexArrays(1, &vao_);
    }
    if (font_texture_ != 0U) {
        glDeleteTextures(1, &font_texture_);
    }

    shader_program_ = 0;
    vbo_ = 0;
    vao_ = 0;
    font_texture_ = 0;
}

void GlyphRenderer::render_text(const char* glyphs, const uint32_t cols,
                                const uint32_t rows) {
    glUseProgram(shader_program_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font_texture_);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUniformMatrix4fv(u_projection_loc_, 1, GL_TRUE,
                       projection_matrix_.data());

    // Precompute texture coordinate scales
    const float uv_scale_x = 1.F / static_cast<float>(atlas_cols_);
    const float uv_scale_y = 1.F / static_cast<float>(atlas_rows_);

    uint32_t cursor_x = cols;
    const uint32_t cursor_y = rows;
    for (const char* ptr = glyphs; *ptr != 0; ptr++, ++cursor_x) {
        const uint8_t glyph_code = static_cast<uint8_t>(*ptr);
        const uint32_t tile_x = glyph_code % atlas_cols_;
        const uint32_t tile_y = glyph_code / atlas_cols_;

        const float min_u = static_cast<float>(tile_x) * uv_scale_x;
        const float min_v = static_cast<float>(tile_y) * uv_scale_y;
        const float max_u = min_u + uv_scale_x;
        const float max_v = min_v + uv_scale_y;

        const float screen_pos_x = static_cast<float>(cursor_x) * glyph_width_;
        const float screen_pos_y = static_cast<float>(cursor_y) * glyph_height_;
        const float tile_width = glyph_width_;
        const float tile_height = glyph_height_;

        // clang-format off
        std::array<std::array<float,FLOATS_PER_VERTEX>, VERTICES_PER_QUAD> vertices = { {
            { screen_pos_x,                 screen_pos_y + tile_height, min_u, max_v },
            { screen_pos_x,                 screen_pos_y,               min_u, min_v },
            { screen_pos_x + tile_width,    screen_pos_y,               max_u, min_v },
            { screen_pos_x,                 screen_pos_y + tile_height, min_u, max_v },
            { screen_pos_x + tile_width,    screen_pos_y,               max_u, min_v },
            { screen_pos_x + tile_width,    screen_pos_y + tile_height, max_u, max_v },
        }};
        // clang-format on

        glBufferSubData(GL_ARRAY_BUFFER, 0,
                        VERTICES_PER_QUAD * FLOATS_PER_VERTEX * sizeof(float),
                        vertices.data());
        glDrawArrays(GL_TRIANGLES, 0, VERTICES_PER_QUAD);
    }
}
