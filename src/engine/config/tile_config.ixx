//-----------------------------------------------------------------------------
// src/engine/config/tile_config.ixx
//-----------------------------------------------------------------------------
module;
#include <cstdint>

export module Engine.Config.TileConfig;

// The pixel dimensions of one map-tile (adjust to match font atlas)
export constexpr uint32_t TILE_WIDTH  = 8;
export constexpr uint32_t TILE_HEIGHT = 16;

// Single square size
export constexpr uint32_t SQUARE_SIZE = TILE_HEIGHT;