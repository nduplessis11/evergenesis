module;
#include <print>
#include <SDL3_image/SDL_image.h>
#include <glad/gl.h>

module Renderer.Glyph;

uint32_t GlyphRenderer::font_texture_ = 0;

bool GlyphRenderer::init(const char* atlas_path) {
    // Initialize SDL_image of PNG support
    SDL_Surface* surface = IMG_Load(atlas_path);
    if (surface == nullptr) {
        std::println(stderr, "Failed to load font atlas '{}': {}", atlas_path,
                     SDL_GetError());
        return false;
    }

    glGenTextures(1, &font_texture_);
    glBindTexture(GL_TEXTURE_2D, font_texture_);

    // Determine format
    const GLenum format =
        (SDL_BYTESPERPIXEL(surface->format) == 4 ? GL_RGBA : GL_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format,
                 GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    SDL_DestroySurface(surface);

    std::println("[GlyphRenderer] loaded font atlas '{}', texture id {}.",
                 atlas_path, font_texture_);
    return true;
}
auto GlyphRenderer::cleanup() -> void {
    if (font_texture_ != 0) {
        glDeleteTextures(1, &font_texture_);
        font_texture_ = 0;
    }
}
