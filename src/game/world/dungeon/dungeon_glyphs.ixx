export module Game.World.Dungeon.Glyphs;

import Game.World.Dungeon;

export inline auto glyph_for_tile(const TileType tile) -> char {
    switch (tile) {
    case TileType::Floor:
        return '.';
    case TileType::Wall:
        return '#';
    case TileType::Door:
        return '+';
    default:
        return ' ';
    }
}