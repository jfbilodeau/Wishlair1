//
// Created by jfbil on 2023-06-07.
//

#include "nomad/game/Game.hpp"

#include "nomad/game/Window.hpp"

namespace nomad {

Window::Window(Game* game, const String& title, const Point& resolution): m_game(game) {

}

Window::~Window() {

}

} // nomad