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
    float       u;
    float       v;

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
    void clear();

private:
    std::vector<RenderCommand> commands_;
};

struct IRendererBackend {
    virtual ~IRendererBackend()                                      = default;
    virtual void initialize()                                        = 0;
    virtual void load_shader(std::string name, std::string vertex_src,
                             std::string fragment_src)               = 0;
    virtual void execute(const std::vector<RenderCommand>& commands) = 0;
};
} // namespace Renderer
