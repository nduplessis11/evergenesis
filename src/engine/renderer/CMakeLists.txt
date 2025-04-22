add_library(renderer STATIC)

target_sources(renderer
    PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES FILES renderer_core.ixx glyph_renderer.ixx
    PRIVATE renderer_core.cpp
    PRIVATE glyph_renderer.cpp
)

target_link_libraries(renderer
    PRIVATE vendor
    PRIVATE core
)