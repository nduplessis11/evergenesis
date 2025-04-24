//-----------------------------------------------------------------------------
// tile_map_render_system.cpp
//-----------------------------------------------------------------------------
module;
#include <print>

module Engine.Rendering.Systems.TileMap;

TileMapRenderSystem::TileMapRenderSystem(GlyphRenderer& glyph_renderer)
    : glyph_renderer_(glyph_renderer) {}

auto TileMapRenderSystem::update(EcsWorld& world) const -> void {
    // Find all entities with a TileMap component
    for (const auto entity : world.entities_with<TileMap>()) {
        auto& [glyphs, cols, rows] = world.get_component<TileMap>(entity);

        // Sanity check: ensure buffer size matches expected size
        if (const size_t expected_size = static_cast<size_t>(cols) * rows;
            glyphs.size() != expected_size) {
            std::println("WARNING: TileMap buffer size mismatch!");
            continue;
        }

        // Render the tile map using the glyph renderer
        glyph_renderer_.render_console(glyphs.data(), cols, rows);
    }
}