//-----------------------------------------------------------------------------
// dungeon.ixx
//-----------------------------------------------------------------------------
module;
#include <cstdint>
#include <vector>

export module Game.World.Dungeon;

export struct DungeonSize {
    std::size_t width;
    std::size_t height;
};

export enum class TileType : int8_t {
    Unknown = -1, // Default/invalid state
    Floor   = 0,  // Walkable floor tile
    Wall    = 1,  // Solid wall tile that blocks movement
    Door    = 2   // Interactive tile that can be opened/closed
};

export class Dungeon {
public:
    explicit Dungeon(DungeonSize dimensions);

    // Procedural generation or manual fill
    void generate();

    // Accessors
    [[nodiscard]] auto width() const -> size_t;
    [[nodiscard]] auto height() const -> size_t;
    [[nodiscard]] auto tile_at(size_t x_pos, size_t y_pos) const -> TileType;

private:
    size_t width_{};
    size_t height_{};
    std::vector<TileType> tiles_;
};