module;
#include <SDL3/SDL.h>
#include <glad/gl.h>
#include <print>

module Renderer.Core;

constexpr int RENDERER_GL_MAJOR_VERSION = 4;
constexpr int RENDERER_GL_MINOR_VERSION = 6;

RenderSystem::RenderSystem() = default;

RenderSystem::~RenderSystem() {
    cleanup();
}

auto RenderSystem::init(const char* title, const int width, const int height)
                         -> bool {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::println(stderr, "Failed to initialize SDL: {}", SDL_GetError());
        return false;
    }

    // Request an OpenGL 4.6 Core context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,
                        RENDERER_GL_MAJOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,
                        RENDERER_GL_MINOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);

    // Create the window with an OpenGL context
    window_ = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL);

    if (window_ == nullptr) {
        std::println(stderr, "Failed to create window: {}", SDL_GetError());
        return false;
    }

    gl_context_ = SDL_GL_CreateContext(window_);
    if (gl_context_ == nullptr) {
        std::println(stderr,
                     "Failed to create OpenGL context: {}",
                     SDL_GetError());
        return false;
    }

    // Load GL function pointers, e.g. with GLAD
    if (gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress) == 0) {
        std::println(stderr, "Failed to initialize GLAD");
        return false;
    }

    // Set up the viewport to match window size
    int32_t view_width;
    int32_t view_height;
    SDL_GetWindowSize(window_, &view_width, &view_height);
    glViewport(0, 0, view_width, view_height);

    return true;
}
void RenderSystem::cleanup() {
    if (gl_context_ != nullptr) {
        SDL_GL_DestroyContext(gl_context_);
        gl_context_ = nullptr;
    }
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
}

void RenderSystem::swap_buffers() const {
    SDL_GL_SwapWindow(window_);
}