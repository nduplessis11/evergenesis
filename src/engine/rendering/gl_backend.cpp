module;
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.inl>
#include <print>

module Engine.Renderer.GlBackend;

void Renderer::GlBackend::initialize() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set up a shared VAO/VBO for textured quads
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    // Each vertex: 4 floats
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
}
void Renderer::GlBackend::load_shader(const std::string& name,
                                      const std::string& vertex_src,
                                      const std::string& fragment_src) {
    auto compile = [&](const GLenum type, const char* source) {
        const GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status != GL_TRUE) {
            char buffer[512];
            glGetShaderInfoLog(shader, 512, nullptr, buffer);
            std::println(stderr, "Shader compilation failed: {}", buffer);
        }
        return shader;
    };

    const auto vertex_shader = compile(GL_VERTEX_SHADER, vertex_src.c_str());
    const auto fragment_shader =
            compile(GL_FRAGMENT_SHADER, fragment_src.c_str());

    const auto program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        char buffer[512];
        glGetProgramInfoLog(program, 512, nullptr, buffer);
        std::println(stderr, "Shader linking failed: {}", buffer);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    programs_[std::move(name)] = program;
}
void Renderer::GlBackend::load_texture(TextureHandle&   out_texture,
                                       const ImageData& image) {
    glGenTextures(1, &out_texture);
    glBindTexture(GL_TEXTURE_2D, out_texture);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 image.format,
                 image.width,
                 image.height,
                 0,
                 image.format,
                 GL_UNSIGNED_BYTE,
                 image.pixels.data());
}
void Renderer::GlBackend::execute(const std::vector<RenderCommand>& commands) {
    // Batch all textured quads by grouping by shader and texture
    // Handle only TexturedQuads for now
    for (auto const& command : commands) {
        if (command.type != CommandType::TexturedQuad) {
            continue;
        }

        // Bind program
        const auto program_iter = programs_.find(command.shader_name);
        if (program_iter == programs_.end()) {
            continue;
        }
        const auto program = program_iter->second;
        glUseProgram(program);

        // Bind projection
        if (const auto proj_loc = glGetUniformLocation(program, "u_proj"); proj_loc >= 0) {
            glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix_));
        }

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, command.texture);

        // If the shader expects a sampler uniform, set it here:
        if (const auto sampler_loc = glGetUniformLocation(program, "u_tex");
            sampler_loc >= 0) {
            glUniform1i(sampler_loc, 0);
        }

        // Upload vertex buffer
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER,
                     command.vertex_data.size() * sizeof(float),
                     command.vertex_data.data(),
                     GL_DYNAMIC_DRAW);

        // Draw all vertices (6 per quad)
        const GLsizei count =
                static_cast<GLsizei>(command.vertex_data.size() / 4);
        glDrawArrays(GL_TRIANGLES, 0, count);
    }

    // Reset state
    glUseProgram(0);
}