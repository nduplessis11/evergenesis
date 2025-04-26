add_library(game STATIC)

target_sources(game
        PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES FILES
            world/dungeon/dungeon.ixx
            world/dungeon/dungeon_glyphs.ixx
            world/dungeon/systems/dungeon_to_tile_map_system.ixx
            actors/player_factory.ixx
        PRIVATE
            world/dungeon/dungeon.cpp
)

target_link_libraries(game
    PRIVATE engine
    PRIVATE vendor
)