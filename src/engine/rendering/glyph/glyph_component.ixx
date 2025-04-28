module;
#include <string>

export module Engine.Rendering.Glyph:Component;

import Engine.Rendering.Renderer;

export struct GlyphComponent {
    char glyph;
};

export struct GlyphResource {
    Renderer::TextureHandle texture;
    std::string             shader_name;
    uint32_t                atlas_cols;
    uint32_t                atlas_rows;
    float                   glyph_width;
    float                   glyph_height;
};