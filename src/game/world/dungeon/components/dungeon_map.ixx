export module Game.Components.DungeonMap;

import Game.World.Dungeon;

export struct DungeonMap {
    // We store a raw pointer here, so the Dungeon class still owns its data.
    // If we ever need to duplicate or snapshot tiles, we could instead
    // store a std::vector<TileType> and width/height.
    Dungeon* dungeon;
};
