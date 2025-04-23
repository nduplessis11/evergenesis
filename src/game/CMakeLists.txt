add_library(game STATIC)

target_sources(game
        PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES FILES world/dungeon/dungeon.ixx
        PRIVATE world/dungeon/dungeon.cpp
)

#target_link_libraries(renderer
#        PRIVATE vendor
#        PRIVATE core
#)