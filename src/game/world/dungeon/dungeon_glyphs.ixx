export module Game.World.Dungeon.Glyphs;

import Game.World.Dungeon;

export inline char glyph_for_tile(const TileType tile) {
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