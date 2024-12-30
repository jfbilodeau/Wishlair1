//
// Created by jfbil on 2024-12-19.
//

#include "nomad/resource/Text.hpp"

#include "nomad/log/Logger.hpp"

#include "nomad/resource/ResourceManager.hpp"

#include <boost/tokenizer.hpp>

#include <fstream>

namespace nomad {

void TextManager::load_text_from_csv(const NomadString& path) {
    m_language_codes.clear();
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

    { // Read headers
        boost::tokenizer<boost::escaped_list_separator<NomadChar>> tokens(line);

        for (const auto& token: tokens) {
            if (token == "key") {
                // Skip over 'key' column
                continue;
            }

            m_language_codes.push_back(token);
        }
    }

    while (std::getline(file, line)) {
        boost::tokenizer<boost::escaped_list_separator<char>> tokens(line);

        NomadString key;
        auto column_index = -1;

        for (const auto& token : tokens) {
            column_index++;

            if (column_index == 0) {
                key = token;
                continue;
            }

            auto language_code = m_language_codes.at(column_index - 1);

            m_texts[language_code][key] = token;
        }
    }
}

bool TextManager::has_language(const NomadString& language_code) const {
    return m_texts.find(language_code) != m_texts.end();
}

const std::vector<NomadString>& TextManager::get_language_codes() const {
    return m_language_codes;
}

bool TextManager::has_text(const NomadString& language_code, const NomadString& key) const {
    return m_texts.find(language_code) != m_texts.end()
         && m_texts.at(language_code).find(key) != m_texts.at(language_code).end();
}

const NomadString& TextManager::get_text(const NomadString& language_code, const NomadString& key) const {
    return m_texts.at(language_code).at(key);
}

void TextManager::get_all_text(const NomadString& language_code, std::unordered_map<NomadString, NomadString>& texts) const {
    texts = m_texts.at(language_code);
}

} // namespace nomad