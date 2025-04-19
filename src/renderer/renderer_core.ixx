module;
#include <SDL3/SDL.h>

export module Renderer.Core;

export class RenderSystem {
public:
    RenderSystem();
    ~RenderSystem();

    /// Initialize SDL3, create an OpenGL context, and set up the viewport.
    /// Returns true on success.
    auto init(const char* title, int width, int height) -> bool;

    /// Clean up GL context and SDL3 subsystems.
    void cleanup();

    /// Swap the OpenGL buffers (present the back buffer).
    void swap_buffers() const;

private:
    SDL_Window* window_ = nullptr;
    SDL_GLContext gl_context_ = nullptr;
};