module;
#include <array>
#include <print>
#include <utility>
#include <vector>

module Engine.Rendering.Systems.Core;

import Engine.Core;
import Engine.Physics.Components.Transform;
import Engine.Rendering.Components.GlyphRenderable;
import Engine.Rendering.Components.TileMap;
import Engine.Config.TileConfig;
import Engine.Ecs.Entity;

constexpr Color DARK_GREY_COLOR{.r = 0.1F, .g = 0.1F, .b = 0.1F, .a = 1.0F};

RenderSystem::RenderSystem(
        SdlGlGraphicsContext& graphics_context, std::unique_ptr<IRenderer> renderer)
    : graphics_context_(graphics_context), renderer_(std::move(renderer)) {
    // The renderer is initialized and owned by this RenderSystem
}

void RenderSystem::set_world(Registry& world) {
    world_ = &world;
}

void RenderSystem::update(float /*delta_time*/) const {
    // Clear screen to a dark gray background
    SdlGlGraphicsContext::begin_frame(DARK_GREY_COLOR);

    if (world_ != nullptr) {
        bool drew_map = false;

        // Batch renders any tile maps with overlays
        for (Entity entity : world_->entities_with<TileMap>()) {
            auto&       tile_map = world_->get_component<TileMap>(entity);
            const auto& glyphs   = tile_map.glyphs;
            const auto  cols     = tile_map.cols;
            const auto  rows     = tile_map.rows;

            // Sanity check: ensure buffer size matches expected dimensions
            const std::size_t expected_size =
                    static_cast<std::size_t>(cols) * rows;
            if (glyphs.size() != expected_size) {
                std::println("WARNING: TileMap buffer size mismatch!");
                continue;
            }

            // Prepare a combined glyph buffer (copy base map data)
            std::vector<char> combined_glyphs = glyphs;

            // Overlay any glyph renderables on top of the base map
            world_->for_each<Transform, GlyphRenderable>(
                    [&](const Transform&           transform,
                            const GlyphRenderable& glyph) {
                        // compute integer tile coords
                        const auto col_i = static_cast<std::int32_t>(
                                transform.position.x / TILE_WIDTH);
                        const auto row_i = static_cast<std::int32_t>(
                                transform.position.y / TILE_HEIGHT);
                        if (col_i < 0 || row_i < 0) {
                            return;
                        }

                        // switch to unsigned for comparison
                        const auto col_u = static_cast<size_t>(col_i);
                        const auto row_u = static_cast<size_t>(row_i);

                        if (col_u >= cols || row_u >= rows) {
                            return;
                        }

                        // explicit parens to clarify order
                        const auto index       = (row_u * cols) + col_u;
                        combined_glyphs[index] = glyph.glyph;
                    });
            // Draw the combined tile map in one pass
            renderer_->render_console(combined_glyphs.data(), cols, rows);
            drew_map = true;
        }
        if (!drew_map) {
            // No tile map present: render each glyph entity individually
            world_->for_each<Transform, GlyphRenderable>(
                    [&](const Transform&           transform,
                            const GlyphRenderable& glyph) {
                        const auto col = static_cast<std::int32_t>(
                                transform.position.x / TILE_WIDTH);
                        const auto row = static_cast<std::int32_t>(
                                transform.position.y / TILE_HEIGHT);

                        // build a tiny text batch
                        const std::array text = {glyph.glyph, '\0'};
                        renderer_->render_text(text.data(), col, row);
                    });
        }
    } else {
        // Fallback: render error text if the world is not set
        renderer_->render_text("Something went wrong with the world!", 1, 1);
    }
    graphics_context_.end_frame();
}
