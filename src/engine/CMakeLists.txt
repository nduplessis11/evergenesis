add_library(engine STATIC)

target_link_libraries(engine
        PRIVATE vendor
)

add_subdirectory(core)
add_subdirectory(ecs)
add_subdirectory(systems/renderer)