//-----------------------------------------------------------------------------
// src/engine/rendering/components/glyph_renderable.ixx
//-----------------------------------------------------------------------------
export module Engine.Rendering.Components.GlyphRenderable;

export struct GlyphRenderable
{
    char glyph;      // ASCII character (e.g. '@', '#', etc.)
    // could also store color, tint, or UV‐offset here if desired
};