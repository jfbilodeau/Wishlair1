//
// Created by Jean-François Bilodeau on 2023-06-16.
//

#ifndef NOMAD_COMPILEREXCEPTION_HPP
#define NOMAD_COMPILEREXCEPTION_HPP

#include "nomad/nomad.hpp"

namespace nomad {

class CompilerException : public NomadException {
public:
    explicit CompilerException(const NomadString& message) : NomadException(message) {}
};

}

#endif //NOMAD_COMPILEREXCEPTION_HPP
