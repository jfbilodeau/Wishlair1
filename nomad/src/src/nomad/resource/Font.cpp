//
// Created by jfbil on 2023-09-06.
//

#include "nomad/game/Canvas.hpp"

#include "nomad/game/Color.hpp"

#include "nomad/log/Logger.hpp"

#include "nomad/resource/ResourceManager.hpp"
#include "nomad/resource/Font.hpp"

#include "nomad/system/FastHeap.hpp"

#include <SDL.h>

#include <istream>
#include <sstream>

namespace nomad {

///////////////////////////////////////////////////////////////////////////////
// Font
Font::Font(const NomadString& name, const NomadString& file_name, int point_size):
    m_name(name)
{
    m_point_size = point_size;

    m_font = TTF_OpenFont(file_name.c_str(), point_size);

    if (m_font == nullptr) {
        auto error_message = TTF_GetError();

        throw ResourceException("Failed to load font: " + file_name + ". Reason: " + error_message);
    }
}

const NomadString& Font::get_name() const {
    return m_name;
}

Font::~Font() {
    TTF_CloseFont(m_font);
}

int Font::get_point_size() const {
    return m_point_size;
}

TTF_Font* Font::get_ttf_font() const {
    return m_font;
}

SDL_Texture* Font::generate_texture(
    Canvas* canvas,
    const NomadString& text,
    const Color& color,
    HorizontalAlignment alignment,
    NomadInteger max_text_width_pixels,
    NomadInteger max_text_height_pixels,
    NomadInteger line_spacing
) const {
    struct Line {
        TempString text;
        NomadInteger width;
    };

    if (text.empty()) {
        // Create a 1x1 texture
        return SDL_CreateTexture(canvas->get_sdl_renderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1, 1);
    }

    const auto font_height = get_font_height();
    const auto line_height = font_height + line_spacing;

    SDL_Surface* surface;

    if (max_text_width_pixels == 0) {
        surface = TTF_RenderUTF8_Blended_Wrapped(m_font, text.c_str(), color.to_sdl_color(), 0);
    } else {
        std::vector<Line> lines;

        // Pre-allocate strings to avoid allocations at each iteration.
        TempString test_line, current_line, word;
        NomadInteger longest_line_width = 0;

        TempStringStream text_stream(
            reinterpret_cast<const TempString&>(text)
        );

        for (TempString line; std::getline(text_stream, line);) {
            auto current_text_height = static_cast<NomadInteger>(lines.size()) * line_height;

            if (max_text_height_pixels != 0 && current_text_height > max_text_height_pixels) {
                break;
            }

            TempStringStream line_stream(line);

            while (line_stream >> word) {
                test_line.clear();
                test_line.append(current_line).append(current_line.empty() ? "" : " ").append(word);
                auto line_width = get_text_width(test_line.to_nomad_string_ref());

                if (line_width > max_text_width_pixels) {
                    if (!current_line.empty()) {
                        lines.emplace_back(
                            Line{
                                current_line,
                                get_text_width(current_line.to_nomad_string_ref())
                            }
                        );
                        current_line = word; // Start next line with 'word'
                    } else {
                        // Word is too long to fit on a line.
                        lines.emplace_back(
                            Line{
                                word,
                                get_text_width(word.to_nomad_string_ref())
                            }
                        );
                    }

                    // Update max_line_width
                    line_width = get_text_width(lines.back().text.to_nomad_string_ref());
                    longest_line_width = std::max(longest_line_width, line_width);
                } else {
                    current_line = test_line;
                }
            }

            if (!current_line.empty()) {
                lines.emplace_back(
                    Line {
                        current_line,
                        get_text_width(current_line.to_nomad_string_ref())
                    }
                );
                longest_line_width = std::max(longest_line_width, get_text_width(current_line.to_nomad_string_ref()));
            }
        }

        NomadInteger current_text_height = static_cast<NomadInteger>(lines.size()) * line_height;

        auto surface_height = max_text_height_pixels;

        if (surface_height == 0) {
            surface_height = current_text_height;
        }

        auto surface_width = max_text_width_pixels;

        if (surface_width == 0) {
            surface_width = longest_line_width;
        }

        surface = SDL_CreateRGBSurfaceWithFormat(
            0,
            static_cast<int>(surface_width),
            static_cast<int>(surface_height),
            32,
            SDL_PIXELFORMAT_RGBA4444
        );

        for (NomadIndex line_index = 0; line_index < lines.size(); ++line_index) {
            auto& line = lines[line_index];

            auto text_surface = TTF_RenderUTF8_Blended(m_font, line.text.c_str(), color.to_sdl_color());

            SDL_Rect destination_rect;

            switch (alignment) {
                case HorizontalAlignment::Left:
                    destination_rect = SDL_Rect{
                        0,
                        static_cast<int>(line_index * line_height),
                        text_surface->w,
                        text_surface->h
                    };
                    break;

                case HorizontalAlignment::Middle:
                    destination_rect = SDL_Rect{
                        static_cast<int>((surface_width - line.width) / 2),
                        static_cast<int>(line_index * line_height),
                        text_surface->w,
                        text_surface->h
                    };
                    break;

                case HorizontalAlignment::Right:
                    destination_rect = SDL_Rect{
                        static_cast<int>(surface_width - line.width),
                        static_cast<int>(line_index * line_height),
                        text_surface->w,
                        text_surface->h
                    };
                    break;
            }

            SDL_BlitSurface(text_surface, nullptr, surface, &destination_rect);

            SDL_FreeSurface(text_surface);
        }
    }

    auto texture = SDL_CreateTextureFromSurface(canvas->get_sdl_renderer(), surface);

    if (texture == nullptr) {
        auto error_message = SDL_GetError();

        log::error("Failed to create texture from surface. Reason: " + NomadString(error_message));
    }

    SDL_FreeSurface(surface);

    return texture;
}

NomadInteger Font::get_text_width(const NomadString& text) const {
    int width = 0;

    TTF_SizeUTF8(m_font, text.c_str(), &width, nullptr);

    return static_cast<NomadInteger>(width);
}

NomadInteger Font::get_text_height(const NomadString& text) const {
    int height = 0;

    TTF_SizeUTF8(m_font, text.c_str(), nullptr, &height);

    return static_cast<NomadInteger>(height);
}

NomadInteger Font::get_font_height() const {
    int font_height = TTF_FontHeight(m_font);

    return static_cast<NomadInteger>(font_height);
}

///////////////////////////////////////////////////////////////////////////////
// FontManager
FontManager::FontManager(ResourceManager* resources):
    m_resources(resources)
{}

NomadId FontManager::register_font(const NomadString& font_name, int point_size) {
    auto font_id = to_nomad_id(m_fonts.size());

    const auto file_name = m_resources->make_resource_path(font_name);

    m_fonts.emplace_back(
        std::make_unique<Font>(
            font_name,
            file_name,
            point_size
        )
    );

    return font_id;
}

const Font* FontManager::get_font(NomadId font_id) const {
    if (font_id >= m_fonts.size()) {
        return nullptr;
    }

    return m_fonts[font_id].get();
}

const Font* FontManager::get_font_by_name(const NomadString& font_name) const {
    for (const auto& font : m_fonts) {
        if (font->get_name() == font_name) {
            return font.get();
        }
    }

    return nullptr;
}

} // nomand