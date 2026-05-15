# Glyph Rendering: GoF Composite vs ECS

A comparison between the Gang of Four Lexi document editor pattern and the EverGenesis character rendering architecture.

## GoF Lexi — Composite Pattern

The Lexi design uses the **Composite** pattern. `Glyph` is an abstract base that both *leaves* (`Character`, `Rectangle`) and *containers* (`Row`) implement. `Row` holds `children : List<Glyph>` and its `Draw()` iterates them, delegating to `c->Draw(w)` on each child. The key property: a single character and an entire row of characters are treated identically through the same interface.

Responsibility sits **inside the object** — `Character::Draw()` tells the `Window` to draw itself (`w->DrawCharacter(c)`). The rendering context (`Window`) is passed down the tree.

## EverGenesis — ECS + Batch Rendering

Responsibility sits **outside the data**. `GlyphRenderable` is a plain struct (`char glyph`) with no `Draw()` method and no knowledge of rendering. `RenderSystem` owns all drawing logic — it queries the registry, accumulates every glyph's quad vertices into one buffer, and issues a single `glDrawArrays()`.

## Side-by-side

| | GoF Lexi | EverGenesis |
|---|---|---|
| **Pattern** | Composite | ECS + Batch Rendering |
| **Structure** | Object tree (`Row` → `Character`) | Flat component arrays |
| **Draw call count** | 1 per glyph (virtual dispatch) | 1 for all glyphs |
| **Who draws?** | The glyph draws itself | External system draws all |
| **Hit testing** | `Intersects(Point)` on every `Glyph` | Not in the render path |
| **Mixed content** | Yes — chars, shapes, images all as `Glyph` | No — homogeneous char grid |
| **Layout** | Hierarchical, relative positioning | Absolute pixel coords |
| **Extensibility** | Add a new `Glyph` subclass | Add a new component + system |

## Core Philosophical Difference

GoF Lexi is designed for a **rich document editor** — content is heterogeneous (text, shapes, images), layout is hierarchical (pages → rows → chars), and hit-testing for mouse interaction is a first-class concern. The tree structure and virtual dispatch are load-bearing.

EverGenesis is designed for a **tile-based game** — the screen is a fixed-size ASCII grid, all glyphs are the same type, and GPU throughput matters more than flexibility. Virtual dispatch per character would be a performance liability at render time.

The deepest contrast: GoF says *behavior belongs with data* — each object knows how to draw itself. ECS says *behavior is completely separate from data* — systems process dumb structs. They are not just different implementations; they reflect opposite answers to the question of where logic should live.
