module;
#include <cstdint>

export module Engine.Rendering.RendererInterface;

export class IRenderer {
public:
    virtual ~IRenderer() = default;
    // Render a null-terminated string at the given tile coordinates (column, row).
    virtual void render_text(const char* text, std::int32_t start_col, std::int32_t start_row) const = 0;
    // Render an entire console of size cols × rows using a buffer of glyphs.
    virtual void render_console(const char* glyphs, std::uint32_t cols, std::uint32_t rows) const = 0;
};
