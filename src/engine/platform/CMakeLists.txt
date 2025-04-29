target_sources(engine
    PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES BASE_DIRS . FILES
        sdl/sdl_gl_graphics_context.ixx
        sdl/sdl_image_loader.ixx
        sdl/sdl_platform.ixx
    PRIVATE
        sdl/sdl_gl_graphics_context.cpp
)