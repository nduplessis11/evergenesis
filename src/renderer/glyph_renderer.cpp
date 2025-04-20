module;
#include <array>
#include <print>
#include <SDL3_image/SDL_image.h>
#include <glad/gl.h>

module Renderer.Glyph;

constexpr uint32_t PROJECTION_MATRIX_SIZE = 16;
constexpr uint32_t GLYPH_WIDTH = 8;
constexpr uint32_t GLYPH_HEIGHT = 16;
constexpr uint32_t ATLAS_COLS = 16;
constexpr uint32_t ATLAS_ROWS = 16;

uint32_t GlyphRenderer::font_texture_ = 0;
uint32_t GlyphRenderer::vao_ = 0;
uint32_t GlyphRenderer::vbo_ = 0;
uint32_t GlyphRenderer::shader_program_ = 0;
uint32_t GlyphRenderer::glyph_width_ = GLYPH_WIDTH;
uint32_t GlyphRenderer::glyph_height_ = GLYPH_HEIGHT;
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

auto GlyphRenderer::init(const char* atlas_path) -> bool {
    // Initialize SDL_image of PNG support
    SDL_Surface* surface = IMG_Load(atlas_path);
    if (surface == nullptr) {
        std::println(stderr, "Failed to load font atlas '{}': {}", atlas_path,
                     SDL_GetError());
        return false;
    }
    screen_width_ = surface->w;
    screen_height_ = surface->h;
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

    // VAO/VBO for quad (x,y,u,v)
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    constexpr size_t VERTICES_PER_QUAD = 6;
    constexpr size_t FLOATS_PER_VERTEX = 4;
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(float) * VERTICES_PER_QUAD * FLOATS_PER_VERTEX, nullptr,
                 GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

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

void GlyphRenderer::render_text(const char* text, int x, int y) {
    glUseProgram(shader_program_);
    glBindVertexArray(vao_);
    glBindTexture(GL_TEXTURE_2D, font_texture_);

    const float width = static_cast<float>(screen_width_);
    const float height = static_cast<float>(screen_height_);

    // Orthographic projection: assume unit = pixel
    // clang-format off
    std::array proj = {
        2.F/width,  0.F,         0.F, -1.F,     // NOLINT(*-magic-numbers)
        0.F,        2.F/height,  0.F, -1.F,     // NOLINT(*-magic-numbers)
        0.F,        0.F,        -1.F,  0.F,
        0.F,        0.F,         0.F,  1.F
    };
    // clang-format on
}
