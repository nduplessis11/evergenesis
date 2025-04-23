set(SDL_TESTS OFF)
set(SDL_TEST_LIBRARY OFF)
add_subdirectory(SDL)

# Turn off vendored dependencies before including SDL_image
set(SDLIMAGE_VENDORED OFF)
add_subdirectory(SDL_image)

add_subdirectory(glad/cmake)
glad_add_library(glad_gl_core_46 STATIC API gl:core=4.6)

set(GLM_BUILD_LIBRARY ON)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(glm)

add_library(vendor INTERFACE)

target_link_libraries(vendor
    INTERFACE
        SDL3::SDL3
        SDL3_image::SDL3_image
        glad_gl_core_46
        glm::glm
)