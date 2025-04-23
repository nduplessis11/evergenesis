//-----------------------------------------------------------------------------
// dungeon.cpp
//-----------------------------------------------------------------------------
module;
#include <cstdint>
module Game.World.Dungeon;

Dungeon::Dungeon(const DungeonSize dimensions)
    : width_(dimensions.width), height_(dimensions.height) {
    tiles_.reserve(width_ * height_);
}

void Dungeon::generate() {
    // Example: fill the entire dungeon with Floor except boundary with Walls
    for (uint32_t y_idx = 0; y_idx < height_; ++y_idx) {
        for (uint32_t x_idx = 0; x_idx < width_; ++x_idx) {
            const bool is_boundary = (x_idx == 0 || x_idx == width_ - 1 ||
                                      y_idx == 0 || y_idx == height_ - 1);
            tiles_[(y_idx * width_) + x_idx] = is_boundary ? TileType::Wall
                                                           : TileType::Floor;
        }
    }
}

auto Dungeon::width() const -> size_t {
    return width_;
}

auto Dungeon::height() const -> size_t {
    return height_;
}

auto Dungeon::tile_at(const size_t x_pos, const size_t y_pos) const
                         -> TileType {
    if (x_pos >= width_ || y_pos >= height_) {
        return TileType::Unknown;
    }
    return tiles_[(y_pos * width_) + x_pos];
}
