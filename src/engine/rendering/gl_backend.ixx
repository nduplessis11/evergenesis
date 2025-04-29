module;

#include "glm/mat4x4.hpp"
#include <print>
#include <string>
#include <unordered_map>

export module Engine.Renderer.GlBackend;

import Engine.Rendering.Renderer;
import Engine.Platform.Sdl;

export namespace Renderer {
class GlBackend : public IRendererBackend {
public:
    void initialize() override;

    void load_shader(const std::string& name, const std::string& vertex_src,
                     const std::string& fragment_src) override;

    static void load_texture(TextureHandle&   out_texture,
                             const ImageData& image);

    void execute(const std::vector<RenderCommand>& commands) override;

    void set_projection_matrix(const glm::mat4& matrix) {
        projection_matrix_ = matrix;
    }

private:
    std::unordered_map<std::string, uint32_t> programs_;
    uint32_t                                  vao_{0};
    uint32_t                                  vbo_{0};
    glm::mat4                                 projection_matrix_{1.F};
};
} // namespace Renderer
