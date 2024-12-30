//
// Created by jfbil on 2023-06-05.
//

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#define SDL_MAIN_HANDLED
#include <SDL.h> // For SDL_main()

extern "C"
int SDL_main(int argc, char** argv) {
    doctest::Context context(argc, argv);

    int res = context.run();

    return res;
}

