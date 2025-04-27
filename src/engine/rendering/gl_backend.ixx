module;
#include "SDL3/SDL_opengl.h"

#include <glad/gl.h>
#include <string>
#include <unordered_map>

export module Engine.Renderer:GlBackend;

import Engine.Rendering.Renderer;

export namespace Renderer {
class GlBackend : public IRendererBackend {
public:
    void initialize() override {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Set up a shared VAO/VBO for textured quads
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);

        // Each vertex: 4 floats
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
                0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    }

    void load_texture(TextureHandle& out_texture) {}

private:
    std::unordered_map<std::string, GLuint> programs_;
    GLuint                                  vao_{0};
    GLuint                                  vbo_{0};
};
} // namespace Renderer
