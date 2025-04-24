// src/engine/ecs/components/tile_map.ixx
module;
#include <vector>
#include <cstdint>

export module Engine.Rendering.Components.TileMap;

// A completely generic grid of glyphs.
// Engine code only depends on its own modules.
export struct TileMap {
    std::vector<char> glyphs; // contiguous buffer of glyph characters
    uint32_t          cols; // width  (number of columns)
    uint32_t          rows; // height (number of rows)
};