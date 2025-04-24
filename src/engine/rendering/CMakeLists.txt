target_sources(engine
    PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES BASE_DIRS . FILES
        systems/renderer_system.ixx
        systems/tile_map_render_system.ixx
        glyph_renderer.ixx
        components/tile_map.ixx
    PRIVATE
        systems/renderer_system.cpp
        systems/tile_map_render_system.cpp
        glyph_renderer.cpp
)