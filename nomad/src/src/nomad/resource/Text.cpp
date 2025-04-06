//
// Created by jfbil on 2024-12-19.
//

#include "nomad/resource/Text.hpp"

#include "nomad/log/Logger.hpp"

#include "nomad/resource/ResourceManager.hpp"

#include <boost/tokenizer.hpp>

#include <fstream>

#include "boost/algorithm/string/trim.hpp"

namespace nomad {

void TextManager::loadTextFromCsv(const NomadString& path) {
    m_languageCodes.clear();
    m_texts.clear();

    std::ifstream file(path);
    if (!file.is_open()) {
        NomadString message = "Could not open text file: " + path;
        throw ResourceException(message);
    }

    NomadString line;

    if (!std::getline(file, line)) {
        throw ResourceException("Could not read header (first) line from text file: " + path);
    }

    boost::trim(line);

    { // Read headers
        boost::tokenizer<boost::escaped_list_separator<NomadChar>> tokens(line);

        for (const auto& token: tokens) {
            if (token == "key") {
                // Skip over 'key' column
                continue;
            }

            m_languageCodes.push_back(token);
        }
    }

    while (std::getline(file, line)) {
        boost::trim(line);

        boost::tokenizer<boost::escaped_list_separator<char>> tokens(line);

        NomadString key;
        auto column_index = -1;

        for (const auto& token : tokens) {
            column_index++;

            if (column_index == 0) {
                key = token;
                continue;
            }

            auto language_code = m_languageCodes.at(column_index - 1);

            m_texts[language_code][key] = token;
        }
    }
}

bool TextManager::hasLanguage(const NomadString& languageCode) const {
    return m_texts.find(languageCode) != m_texts.end();
}

const std::vector<NomadString>& TextManager::getLanguageCodes() const {
    return m_languageCodes;
}

bool TextManager::hasText(const NomadString& languageCode, const NomadString& key) const {
    return
        m_texts.find(languageCode) != m_texts.end() &&
        m_texts.at(languageCode).find(key) != m_texts.at(languageCode).end();
}

NomadString& TextManager::getText(const NomadString& languageCode, const NomadString& key, NomadString& text) const {
    if (hasText(languageCode, key)) {
        text = m_texts.at(languageCode).at(key);
    } else {
        text = "[" + languageCode + ":" + key + "]";
    }

    return text;
}

void TextManager::getAllText(const NomadString& languageCode, std::unordered_map<NomadString, NomadString>& texts) const {
    texts = m_texts.at(languageCode);
}

} // namespace nomad