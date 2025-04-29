module;
#include <cstdint>
#include <string>
#include <vector>

export module Engine.Rendering.Renderer;

import Engine.Core;

export namespace Renderer {
using TextureHandle = uint32_t;

enum class CommandType : uint8_t { SolidRect = 0, TexturedQuad = 1 };

struct RenderCommand {
    CommandType type;
    float       x;
    float       y;
    float       w;
    float       h;

    Color              color;
    TextureHandle      texture;
    std::string        shader_name;
    std::vector<float> vertex_data;
};

class RendererFrontend {
public:
    void submit(const RenderCommand& command) {
        commands_.push_back(command);
    };
    [[nodiscard]] auto get_commands() const {
        return commands_;
    };
    void clear() noexcept {
        commands_.clear();
    };

private:
    std::vector<RenderCommand> commands_;
};

struct IRendererBackend {
    virtual ~IRendererBackend()                                      = default;

    /// Initialize graphics state (shaders, VAO, blending, etc.)
    virtual void initialize()                                        = 0;

    /// Load and compile a shader program under a given name
    virtual void load_shader(const std::string& name,
                             const std::string& vertex_src,
                             const std::string& fragment_src)        = 0;

    /// Execute all queued RenderCommands
    virtual void execute(const std::vector<RenderCommand>& commands) = 0;
};
} // namespace Renderer
