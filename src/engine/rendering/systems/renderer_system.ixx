module;
#include <memory>

export module Engine.Rendering.Systems.Core;

import Engine.Core; // for GraphicsContext and Color
import Engine.Ecs.Registry; // ECS Registry
import Engine.Rendering.RendererInterface; // IRenderer (frontend)

export class RenderSystem {
public:
    RenderSystem(GraphicsContext&      graphics_context,
            std::unique_ptr<IRenderer> renderer);
    void set_world(Registry& world);
    void update(float delta_time) const;

private:
    GraphicsContext& graphics_context_; // not owned (window/GL context)
    std::unique_ptr<IRenderer> renderer_; // owned rendering backend
    Registry*                  world_ = nullptr; // not owned (ECS registry)
};
