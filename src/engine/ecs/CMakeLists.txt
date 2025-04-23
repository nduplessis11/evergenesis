add_library(ecs STATIC)

target_sources(ecs
        PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES FILES
            entity.ixx
            component_storage.ixx
            ecs_world.ixx
        PRIVATE
            entity.cpp
            ecs_world.cpp
)