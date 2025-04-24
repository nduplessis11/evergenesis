//-----------------------------------------------------------------------------
// renderer_system.ixx
//-----------------------------------------------------------------------------
export module Engine.Rendering.Systems.Core;

import Engine.Core;
import Engine.Rendering.GlyphRenderer;
import Engine.Ecs.World;
import Engine.Rendering.Systems.TileMap;

export class RenderSystem {
public:
    // Pass the GraphicsContext by reference since it's owned elsewhere
    RenderSystem(GraphicsContext& graphics_context,
                 GlyphRenderer&&  glyph_renderer);

    // Set the ECS world the render system will query
    void set_world(EcsWorld& world);

    void update(float delta_time);

private:
    GraphicsContext& graphics_context_; // Not owned
    GlyphRenderer    glyph_renderer_; // Owned by this system
    EcsWorld*        world_ = nullptr;

    // Composition of sub-render systems
    TileMapRenderSystem tile_map_renderer_;
};
