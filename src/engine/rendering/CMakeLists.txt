target_sources(engine
    PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES BASE_DIRS ${CMAKE_CURRENT_LIST_DIR} FILES
        components/tile_map.ixx
        components/glyph_renderable.ixx
        rendering_interface.ixx
        renderer.ixx
        glyph/glyph.ixx
        glyph/glyph_component.ixx
        glyph/glyph_render_system.ixx
        gl_backend.ixx
    PRIVATE
        glyph/glyph_render_system.cpp
        gl_backend.cpp
)