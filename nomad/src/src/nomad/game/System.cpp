//
// Created by jfbil on 2023-09-19.
//

#include <utility>

#include "nomad/game/System.hpp"

namespace nomad {

nomad::System::System(std::shared_ptr<Logger> logger):
    m_logger(std::move(logger))
{
    m_logger->debug("Initializing system");
}

System::~System() {

}

std::shared_ptr<Logger> System::get_logger() const {
    return m_logger;
}

} // nomad