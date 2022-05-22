#include <iostream>

#include "SDL.h"

#include "game.hpp"

int main(int argc, char *argv[])
{
    j0g0::Game game;
    j0g0::RenderingContext context;

    context.init(320*4, 180*4, 4);

    game.init( &context );
    game.run();
}