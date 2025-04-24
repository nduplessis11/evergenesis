target_sources(engine
    PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_LIST_DIR} FILES renderer_core.ixx glyph_renderer.ixx
    PRIVATE renderer_core.cpp
    PRIVATE glyph_renderer.cpp
)