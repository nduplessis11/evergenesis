module;
#include <utility>

module Engine.Rendering.Glyph;

import Engine.Physics.Components.Transform;

GlyphRenderSystem::GlyphRenderSystem(Renderer::RendererFrontend& frontend,
                                     GlyphResource               resource)
    : frontend_(frontend), resource_(std::move(resource)) {}

void GlyphRenderSystem::update(Registry& registry) {
    // Precompute UV scale from atlas dimensions
    const float uv_scale_x = 1.f / static_cast<float>(resource_.atlas_cols);
    const float uv_scale_y = 1.f / static_cast<float>(resource_.atlas_rows);

    // Tile size in pixels
    const float tile_w = resource_.glyph_width;
    const float tile_h = resource_.glyph_height;

    // Iterate over entities having both Transform and GlyphComponent
    registry.for_each<Transform, GlyphComponent>([&](auto& transform,
                                                     auto& glyph_component) {
        // Compute UVs
        const uint8_t glyph_code = static_cast<uint8_t>(glyph_component.glyph);

        // Determine the glyph's tile position in the atlas grid
        const uint32_t tile_x_idx = glyph_code % resource_.atlas_cols;
        const uint32_t tile_y_idx = glyph_code / resource_.atlas_cols;

        // Calculate UV coordinates in normalized [0,1] space
        const float min_u = static_cast<float>(tile_x_idx) * uv_scale_x;
        const float min_v = static_cast<float>(tile_y_idx) * uv_scale_y;
        const float max_u = min_u + uv_scale_x;
        const float max_v = min_v + uv_scale_y;

        // Compute screen position of the glyph
        const float pos_x = static_cast<float>(transform.position.x) * tile_w;
        const float pos_y = static_cast<float>(transform.position.y) * tile_h;

        // TODO: Use the `GlyphRenderer::render_console` as reference. Currently
        // using render_text which is incorrect
    });
}