//
// Created by jfbil on 2023-09-19.
//

#pragma once

#include "nomad/log/Logger.hpp"

#include "nomad/game/Game.hpp"
#include "nomad/geometry/Point.hpp"

namespace nomad {

class System {
public:
    System(std::shared_ptr<Logger> logger);
    System(const System&) = delete;
    ~System();

    [[nodiscard]] std::shared_ptr<Logger> getLogger() const;

private:
    std::shared_ptr<Logger> m_logger;
    Game* m_game;
};

} // nomad

