//
// Created by jfbil on 2023-09-06.
//

#include "nomad/game/Canvas.hpp"

#include "nomad/game/Color.hpp"

#include "nomad/log/Logger.hpp"

#include "nomad/resource/ResourceManager.hpp"
#include "nomad/resource/Font.hpp"

#include "nomad/system/TempHeap.hpp"

#include <SDL.h>

#include <istream>

namespace nomad {

///////////////////////////////////////////////////////////////////////////////
// Font
Font::Font(const NomadString& name, const NomadString& fileName, int pointSize):
    m_name(name)
{
    m_pointSize = pointSize;

    m_font = TTF_OpenFont(fileName.c_str(), pointSize);

    if (m_font == nullptr) {
        auto error_message = TTF_GetError();

        throw ResourceException("Failed to load font: " + fileName + ". Reason: " + error_message);
    }
}

const NomadString& Font::getName() const {
    return m_name;
}

Font::~Font() {
    TTF_CloseFont(m_font);
}

int Font::getPointSize() const {
    return m_pointSize;
}

TTF_Font* Font::getTtfFont() const {
    return m_font;
}

Texture* Font::generateTexture(
    Canvas* canvas,
    const NomadString& text,
    const Color& color,
    HorizontalAlignment alignment,
    NomadInteger maxTextWidthPixels,
    NomadInteger maxTextHeightPixels,
    NomadInteger lineSpacing
) const {
    struct Line {
        TempString text;
        NomadInteger width;
    };

    if (text.empty()) {
        // Create a 1x1 texture
        return new Texture("", canvas->getGame(), 1, 1);
    }

    const auto font_height = getFontHeight();
    const auto line_height = font_height + lineSpacing;

    SDL_Surface* surface;

    if (maxTextWidthPixels == 0) {
        surface = TTF_RenderUTF8_Blended_Wrapped(m_font, text.c_str(), color.toSdlColor(), 0);
    } else {
        // Pre-allocate strings to avoid allocations at each iteration.
        auto test_line = createTempString();
        auto current_line = createTempString();
        NomadInteger longest_line_width = 0;

        auto lines = createTempVector<Line>();

        auto split_text = createTempStringVector();
        splitLines(createTempString(text), split_text);

        for (const auto& line : split_text) {
            auto current_text_height = static_cast<NomadInteger>(lines.size()) * line_height;

            if (maxTextHeightPixels != 0 && current_text_height > maxTextHeightPixels) {
                break;
            }

            auto words = createTempStringVector();
            split(line, " ", words);

            for (const auto& word : words) {
                test_line.clear();
                test_line.append(current_line).append(current_line.empty() ? "" : " ").append(word);
                auto line_width = getTextWidth(test_line.c_str());

                if (line_width > maxTextWidthPixels) {
                    if (!current_line.empty()) {
                        lines.emplace_back(
                            Line{
                                current_line,
                                getTextWidth(current_line.c_str())
                            }
                        );
                        current_line = word; // Start next line with 'word'
                    } else {
                        // Word is too long to fit on a line.
                        lines.emplace_back(
                            Line{
                                word,
                                getTextWidth(word.c_str())
                            }
                        );
                    }

                    // Update max_line_width
                    line_width = getTextWidth(lines.back().text.c_str());
                    longest_line_width = std::max(longest_line_width, line_width);
                } else {
                    current_line = test_line;
                }
            }

            if (!current_line.empty()) {
                lines.emplace_back(
                    Line {
                        current_line,
                        getTextWidth(current_line.c_str())
                    }
                );
                longest_line_width = std::max(longest_line_width, getTextWidth(current_line.c_str()));
            }
        }

        NomadInteger current_text_height = static_cast<NomadInteger>(lines.size()) * line_height;

        auto surface_height = maxTextHeightPixels;

        if (surface_height == 0) {
            surface_height = current_text_height;
        }

        auto surface_width = maxTextWidthPixels;

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

            auto text_surface = TTF_RenderUTF8_Blended(m_font, line.text.c_str(), color.toSdlColor());

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

    auto texture = SDL_CreateTextureFromSurface(canvas->getSdlRenderer(), surface);

    if (texture == nullptr) {
        auto error_message = SDL_GetError();

        log::error("Failed to create texture from surface. Reason: " + NomadString(error_message));
    }

    SDL_FreeSurface(surface);

    return new Texture(text, texture);
}

NomadInteger Font::getTextWidth(const NomadChar* text) const {
    int width = 0;

    TTF_SizeUTF8(m_font, text, &width, nullptr);

    return static_cast<NomadInteger>(width);
}

NomadInteger Font::getTextWidth(const NomadString& text) const {
    return getTextWidth(text.c_str());
}

NomadInteger Font::getTextHeight(const NomadString& text) const {
    int height = 0;

    TTF_SizeUTF8(m_font, text.c_str(), nullptr, &height);

    return height;
}

NomadInteger Font::getFontHeight() const {
    int font_height = TTF_FontHeight(m_font);

    return font_height;
}

///////////////////////////////////////////////////////////////////////////////
// FontManager
FontManager::FontManager(ResourceManager* resources):
    m_resources(resources)
{}

NomadId FontManager::registerFont(const NomadString& fontName, int pointSize) {
    auto font_id = to_nomad_id(m_fonts.size());

    const auto file_name = m_resources->makeResourcePath(fontName);

    m_fonts.emplace_back(
        std::make_unique<Font>(
            fontName,
            file_name,
            pointSize
        )
    );

    return font_id;
}

const Font* FontManager::getFont(NomadId fontId) const {
    if (fontId >= m_fonts.size()) {
        return nullptr;
    }

    return m_fonts[fontId].get();
}

const Font* FontManager::getFontByName(const NomadString& fontName) const {
    for (const auto& font : m_fonts) {
        if (font->getName() == fontName) {
            return font.get();
        }
    }

    return nullptr;
}

} // nomand