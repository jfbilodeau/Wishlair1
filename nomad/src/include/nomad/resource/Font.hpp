//
// Created by jfbil on 2023-09-06.
//

#ifndef NOMAD_FONT_HPP
#define NOMAD_FONT_HPP

#include "nomad/game/Alignment.hpp"
#include "nomad/game/Color.hpp"

#include "SDL_ttf.h"

namespace nomad {

// Forward declarations
class Canvas;
class Resource;
class Texture;

///////////////////////////////////////////////////////////////////////////////
// Font
///////////////////////////////////////////////////////////////////////////////
class Font {
public:
    explicit Font(const NomadString& name, const NomadString& file_name, int point_size);
    Font(const Font& other) = delete;
    ~Font();

    [[nodiscard]] const NomadString& get_name() const;
    [[nodiscard]] int get_point_size() const;
    [[nodiscard]] TTF_Font* get_ttf_font() const;

    Texture* generate_texture(
        Canvas* canvas,
        const NomadString& text,
        const Color& color,
        HorizontalAlignment alignment,
        NomadInteger max_text_width_pixels,
        NomadInteger max_text_height_pixels,
        NomadInteger line_spacing
    ) const;

    [[nodiscard]] NomadInteger get_text_width(const NomadChar* text) const;
    [[nodiscard]] NomadInteger get_text_width(const NomadString& text) const;
    [[nodiscard]] NomadInteger get_text_height(const NomadString& text) const;
    [[nodiscard]] NomadInteger get_font_height() const;

private:
    NomadString m_name;
    TTF_Font* m_font = nullptr;
    int m_point_size = 0;
};

///////////////////////////////////////////////////////////////////////////////
// FontManager
///////////////////////////////////////////////////////////////////////////////
class FontManager {
public:
    explicit FontManager(ResourceManager* resources);

    [[nodiscard]] NomadId register_font(const NomadString& font_name, int point_size);
    [[nodiscard]] const Font* get_font(NomadId font_id) const;
    [[nodiscard]] const Font* get_font_by_name(const NomadString& font_name) const;

private:
    ResourceManager* m_resources;
    std::vector<std::unique_ptr<Font>> m_fonts;
};

} // nomad

#endif //NOMAD_FONT_HPP
