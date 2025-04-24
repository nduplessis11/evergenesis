//-----------------------------------------------------------------------------
// tile_map_render_system.ixx
//-----------------------------------------------------------------------------
export module Engine.Rendering.Systems.TileMap;

import Engine.Ecs.World;
import Engine.Rendering.Components.TileMap;
import Engine.Rendering.GlyphRenderer;

export class TileMapRenderSystem {
public:
    explicit TileMapRenderSystem(GlyphRenderer& glyph_renderer);

    // Called each frame to render all tile maps in the world
    void update(EcsWorld& world) const;

private:
    GlyphRenderer& glyph_renderer_;
};