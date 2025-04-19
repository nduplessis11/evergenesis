set(SDL_TESTS OFF)
set(SDL_TEST_LIBRARY OFF)
add_subdirectory(SDL)

add_subdirectory(glad/cmake)
glad_add_library(glad_gl_core_46 STATIC API gl:core=4.6)

add_library(vendor INTERFACE)

target_link_libraries(vendor
        INTERFACE SDL3::SDL3
        INTERFACE glad_gl_core_46
)