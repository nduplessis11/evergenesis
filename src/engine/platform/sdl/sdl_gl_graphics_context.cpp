module;
#include <SDL3/SDL.h>
#include <glad/gl.h>
#include <optional>
#include <print>
#include <string>

module Engine.Platform.Sdl;

constexpr int RENDERER_GL_MAJOR_VERSION = 4;
constexpr int RENDERER_GL_MINOR_VERSION = 6;

//------------------------------------------------------------------------------
// Factory
//------------------------------------------------------------------------------
auto SdlGlGraphicsContext::create(const std::string& title, const int width,
                             const int height)
                         -> std::optional<SdlGlGraphicsContext> {
    SdlGlGraphicsContext instance;
    if (!instance.init(title, width, height)) {
        return std::nullopt;
    }
    // This calls the *move constructor* of GraphicsContext when constructing
    // the optional
    return std::make_optional<SdlGlGraphicsContext>(std::move(instance));
}

//------------------------------------------------------------------------------
// Move Constructor
//------------------------------------------------------------------------------
SdlGlGraphicsContext::SdlGlGraphicsContext(SdlGlGraphicsContext&& other) noexcept {
    // Steal the pointers
    window_     = other.window_;
    gl_context_ = other.gl_context_;

    // Null out the old object so it won't destroy them
    other.window_     = nullptr;
    other.gl_context_ = nullptr;
}

//------------------------------------------------------------------------------
// Move Assignment
//------------------------------------------------------------------------------
auto SdlGlGraphicsContext::operator=(SdlGlGraphicsContext&& other) noexcept
                         -> SdlGlGraphicsContext& {
    if (this != &other) {
        // Clean up our current resources, if any
        cleanup();

        // Steal from 'other'
        window_     = other.window_;
        gl_context_ = other.gl_context_;

        // Null out 'other'
        other.window_     = nullptr;
        other.gl_context_ = nullptr;
    }
    return *this;
}

//------------------------------------------------------------------------------
// Destructor (no SDL_Quit here if you plan to move contexts around)
SdlGlGraphicsContext::~SdlGlGraphicsContext() {
    cleanup();
}

//------------------------------------------------------------------------------
// Cleanup only frees the window/context if they're valid
void SdlGlGraphicsContext::cleanup() {
    if (gl_context_ != nullptr) {
        SDL_GL_DestroyContext(gl_context_);
        gl_context_ = nullptr;
    }
    if (window_ != nullptr) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }

    // If you do *not* plan to move contexts and have exactly one context
    // in your entire app, you *could* call SDL_Quit() here.
    // But to avoid shutting down SDL globally for any new owner,
    // keep SDL_Quit() out.
    //
    // SDL_Quit();
}

//------------------------------------------------------------------------------
// Frame functions
//------------------------------------------------------------------------------
void SdlGlGraphicsContext::begin_frame(const Color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SdlGlGraphicsContext::end_frame() const {
    SDL_GL_SwapWindow(window_);
}

//------------------------------------------------------------------------------
// Init
//------------------------------------------------------------------------------
auto SdlGlGraphicsContext::init(const std::string& title, const int width,
                           const int height) -> bool {
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
    window_ = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL);
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

    // Load GL function pointers (GLAD)
    if (gladLoadGL((GLADloadfunc) SDL_GL_GetProcAddress) == 0) {
        std::println(stderr, "Failed to initialize GLAD");
        return false;
    }

    // Set up the viewport
    int32_t view_width;
    int32_t view_height;
    SDL_GetWindowSize(window_, &view_width, &view_height);
    glViewport(0, 0, view_width, view_height);

    return true;
}