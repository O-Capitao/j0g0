#include "SDL.h"

#include "game.hpp"

int main(int argc, char *argv[])
{

    std::string config_file_name = "config/roboboy_1.yaml";

    j0g0::Game game;
    j0g0::RenderingContext context;

    // TODO - move to config file...
    context.init(config_file_name);

    game.init( &context, config_file_name );
    game.run();
}