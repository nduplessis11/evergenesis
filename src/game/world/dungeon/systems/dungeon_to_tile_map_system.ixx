module;
#include <vector>
#include <print>

export module Game.World.Dungeon.Systems.DungeonToTileMap;

import Engine.Ecs.Registry;
import Engine.Rendering.Components.TileMap;
import Game.World.Dungeon;
import Game.World.Dungeon.Glyphs;

export class DungeonToTileMapSystem {
public:
    explicit DungeonToTileMapSystem(Dungeon& dungeon)
        : dungeon_(dungeon) {}

    void initialize(Registry& world) const {
        const uint32_t cols = static_cast<uint32_t>(dungeon_.width());
        const uint32_t rows = static_cast<uint32_t>(dungeon_.height());

        std::vector<char> glyphs;
        glyphs.reserve(cols * rows);

        for (size_t y = 0; y < rows; ++y) {
            for (size_t x = 0; x < cols; ++x) {
                const auto tile = dungeon_.tile_at(x, y);
                glyphs.push_back(glyph_for_tile(tile));
            }
        }

        const auto entity = world.create_entity();
        world.add_component<TileMap>(entity, std::move(glyphs), cols, rows);
    }

private:
    Dungeon& dungeon_;
};
