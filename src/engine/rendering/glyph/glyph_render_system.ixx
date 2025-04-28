export module Engine.Rendering.Glyph:System;

import Engine.Rendering.Renderer;
import Engine.Ecs.System;
import Engine.Ecs.Registry;
import :Component;

export class GlyphRenderSystem final : public ISystem {
public:
    GlyphRenderSystem(Renderer::RendererFrontend& frontend, GlyphResource resource);
    void update(Registry& registry) override;

private:
    Renderer::RendererFrontend& frontend_;
    GlyphResource               resource_;
};