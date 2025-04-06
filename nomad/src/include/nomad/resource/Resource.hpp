//
// Created by Jean-François Bilodeau on 2023-06-17.
//

#pragma once

#include "nomad/system/String.hpp"

namespace nomad {

class ResourceManager;

class Resource {
public:
    explicit Resource(const NomadString& name);
    Resource(const Resource& other) = delete;
    virtual ~Resource() = default;

    [[nodiscard]] const NomadString& getName() const { return m_name; }

private:
    NomadString m_name;
};

} // nomad

