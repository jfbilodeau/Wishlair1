//
// Created by jfbil on 2023-06-05.
//

#include <iomanip>
#include <sstream>

#include "nomad/system/String.hpp"

namespace nomad {

NomadString to_string(NomadBoolean value) {
    return value ? "true" : "false";
}

NomadString to_string(NomadId value) {
    return std::to_string(value);
}

NomadString to_string(NomadInteger value) {
    return std::to_string(value);
}

NomadString to_string(NomadFloat value) {
    std::stringstream ss;

    ss << std::setprecision(15) << std::noshowpoint << value;

    return ss.str();
}

NomadString to_string(NomadIndex index) {
    return std::to_string(index);
}

} // nomad