//-----------------------------------------------------------------------------
// graphics_context.ixx
//-----------------------------------------------------------------------------
module;
#include <SDL3/SDL.h>
#include <optional>
#include <string>

export module Engine.Core:Graphics;
import :Types;

export class GraphicsContext {
public:
    // Factory function
    static auto create(const std::string& title, int width, int height)
                             -> std::optional<GraphicsContext>;

    GraphicsContext(const GraphicsContext&)                    = delete;
    auto operator=(const GraphicsContext&) -> GraphicsContext& = delete;

    GraphicsContext(GraphicsContext&&) noexcept;
    auto operator=(GraphicsContext&&) noexcept -> GraphicsContext&;

    ~GraphicsContext();
    void cleanup();

    static void begin_frame(Color color);
    void        end_frame() const;

private:
    // private constructor used by factory
    GraphicsContext() = default;

    auto init(const std::string& title, int width, int height) -> bool;

    SDL_Window*   window_     = nullptr;
    SDL_GLContext gl_context_ = nullptr;
};
