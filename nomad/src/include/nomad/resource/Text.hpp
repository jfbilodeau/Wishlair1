//
// Created by jfbil on 2024-12-19.
//

#ifndef NOMAD_TEXT_HPP
#define NOMAD_TEXT_HPP

#include "nomad/nomad.hpp"

#include <unordered_map>
#include <vector>

namespace nomad {

class TextManager {
public:
    TextManager() = default;

    ~TextManager() = default;

    void load_text_from_csv(const NomadString& path);

    bool has_language(const NomadString& language_code) const;
    [[nodiscard]] const std::vector<NomadString>& get_language_codes() const;

    [[nodiscard]] bool has_text(const NomadString& language_code, const NomadString& key) const;
    NomadString& get_text(const NomadString& language_code, const NomadString& key, NomadString& text) const;

    void get_all_text(const NomadString& language_code, std::unordered_map<NomadString, NomadString>& texts) const;

private:
    std::vector<NomadString> m_language_codes;
    std::unordered_map<NomadString, std::unordered_map<NomadString, NomadString>> m_texts;
};

} // namespace nomad

#endif //NOMAD_TEXT_HPP
