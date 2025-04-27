module;
#include <SDL3/SDL_opengl.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
#include <cstdint>
#include <print>
#include <string>
#include <vector>

export module Engine.Platform.Sdl:ImageLoader;

export struct ImageData {
    uint32_t width{};
    uint32_t height{};
    GLenum   format{};    // GL_RGB or GL_RGBA
    std::vector<uint8_t> pixels;
};

export auto load_image(const std::string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (surface == nullptr) {
        std::println(stderr, "Failed to load image: {}", std::string(SDL_GetError()));
        return ImageData{};
    }
    ImageData image_data;
    image_data.width  = surface->w;
    image_data.height = surface->h;

    // Determine GL format
    if (const uint32_t bpp = SDL_BYTESPERPIXEL(surface->format); bpp == 4) {
        image_data.format = GL_RGBA;
    } else if (bpp == 3) {
        image_data.format = GL_RGB;
    } else {
        std::println(stderr, "Unsupported image format: {}", path);
        SDL_DestroySurface(surface);
        return ImageData{};
    }

    const size_t byte_count = static_cast<size_t>(surface->pitch) * surface->h;
    image_data.pixels.resize(byte_count);
    memcpy(image_data.pixels.data(), surface->pixels, byte_count);
    SDL_DestroySurface(surface);

    return image_data;
}