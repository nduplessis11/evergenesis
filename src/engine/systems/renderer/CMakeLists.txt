target_sources(engine
    PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_LIST_DIR} FILES renderer_system.ixx glyph_renderer.ixx
    PRIVATE renderer_system.cpp
    PRIVATE glyph_renderer.cpp
)