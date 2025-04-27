//-----------------------------------------------------------------------------
// graphics_context.ixx
//-----------------------------------------------------------------------------
module;
#include <SDL3/SDL.h>
#include <optional>
#include <string>

export module Engine.Platform.Sdl;
import Engine.Core;

export class SdlGlGraphicsContext {
public:
    // Factory function
    static auto create(const std::string& title, int width, int height)
                             -> std::optional<SdlGlGraphicsContext>;

    SdlGlGraphicsContext(const SdlGlGraphicsContext&)                    = delete;
    auto operator=(const SdlGlGraphicsContext&) -> SdlGlGraphicsContext& = delete;

    SdlGlGraphicsContext(SdlGlGraphicsContext&&) noexcept;
    auto operator=(SdlGlGraphicsContext&&) noexcept -> SdlGlGraphicsContext&;

    ~SdlGlGraphicsContext();
    void cleanup();

    static void begin_frame(Color color);
    void        end_frame() const;

private:
    // private constructor used by factory
    SdlGlGraphicsContext() = default;

    auto init(const std::string& title, int width, int height) -> bool;

    SDL_Window*   window_     = nullptr;
    SDL_GLContext gl_context_ = nullptr;
};
