module;
#include <memory>

export module Engine.Rendering.Systems.Core;

import Engine.Platform.Sdl; // GraphicsContext (window/GL context)
import Engine.Ecs.Registry; // ECS Registry
import Engine.Rendering.RendererInterface; // IRenderer (frontend)

export class RenderSystem {
public:
    RenderSystem(SdlGlGraphicsContext&      graphics_context,
            std::unique_ptr<IRenderer> renderer);
    void set_world(Registry& world);
    void update(float delta_time) const;

private:
    SdlGlGraphicsContext& graphics_context_; // not owned (window/GL context)
    std::unique_ptr<IRenderer> renderer_; // owned rendering backend
    Registry*                  world_ = nullptr; // not owned (ECS registry)
};
