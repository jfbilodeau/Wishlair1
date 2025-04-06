//
// Created by jfbil on 2023-09-06.
//

#pragma once

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
    explicit Font(const NomadString& name, const NomadString& fileName, int pointSize);
    Font(const Font& other) = delete;
    ~Font();

    [[nodiscard]] const NomadString& getName() const;
    [[nodiscard]] int getPointSize() const;
    [[nodiscard]] TTF_Font* getTtfFont() const;

    Texture* generateTexture(
        Canvas* canvas,
        const NomadString& text,
        const Color& color,
        HorizontalAlignment alignment,
        NomadInteger maxTextWidthPixels,
        NomadInteger maxTextHeightPixels,
        NomadInteger lineSpacing
    ) const;

    [[nodiscard]] NomadInteger getTextWidth(const NomadChar* text) const;
    [[nodiscard]] NomadInteger getTextWidth(const NomadString& text) const;
    [[nodiscard]] NomadInteger getTextHeight(const NomadString& text) const;
    [[nodiscard]] NomadInteger getFontHeight() const;

private:
    NomadString m_name;
    TTF_Font* m_font = nullptr;
    int m_pointSize = 0;
};

///////////////////////////////////////////////////////////////////////////////
// FontManager
///////////////////////////////////////////////////////////////////////////////
class FontManager {
public:
    explicit FontManager(ResourceManager* resources);

    [[nodiscard]] NomadId registerFont(const NomadString& fontName, int pointSize);
    [[nodiscard]] const Font* getFont(NomadId fontId) const;
    [[nodiscard]] const Font* getFontByName(const NomadString& fontName) const;

private:
    ResourceManager* m_resources;
    std::vector<std::unique_ptr<Font>> m_fonts;
};

} // nomad

