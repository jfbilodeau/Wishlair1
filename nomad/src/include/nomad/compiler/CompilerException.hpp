//
// Created by Jean-François Bilodeau on 2023-06-16.
//

#pragma once

#include "nomad/nomad.hpp"

namespace nomad {

class CompilerException : public NomadException {
public:
    explicit CompilerException(const NomadString& message) : NomadException(message) {}
};

}
