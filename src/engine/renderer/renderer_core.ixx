export module Renderer.Core;

import Engine.Core;
import Renderer.Glyph;

export class RenderSystem {
public:
    explicit RenderSystem(GraphicsContext& graphics_context,
                          GlyphRenderer&&  glyph_renderer);
    ~RenderSystem();

    void update(float delta_time) const;

private:
    GraphicsContext& graphics_context_;
    GlyphRenderer    glyph_renderer_;
};