//-----------------------------------------------------------------------------
// renderer_system.ixx
//-----------------------------------------------------------------------------
export module Renderer.Core;

import Engine.Core; // For GraphicsContext
import Renderer.Glyph; // For GlyphRenderer

export class RenderSystem {
public:
    // Pass the GraphicsContext by reference since it's owned elsewhere
    RenderSystem(GraphicsContext& graphics_context,
                 GlyphRenderer&&  glyph_renderer);

    // Let the destructor be default or minimal
    ~RenderSystem() = default;

    // Remove 'const' so we can perform rendering operations that change the
    // state
    void update(float delta_time);

private:
    GraphicsContext& graphics_context_; // Not owned
    GlyphRenderer    glyph_renderer_; // Owned by this system
};
