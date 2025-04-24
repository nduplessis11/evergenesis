target_sources(engine
        PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_LIST_DIR} FILES
            entity.ixx
            component_storage.ixx
            ecs_world.ixx
            components/tile_map.ixx
        PRIVATE
            entity.cpp
            ecs_world.cpp
)