#pragma once

#include <vector>

#include <SDL.h>

#include "graphics/gamegeometry.hpp"
#include "graphics/viewport.hpp"
#include "gamethings/gamething.hpp"
#include "gamethings/playerthing.hpp"

namespace j0g0 {

    struct GameLevelProperties {
        Vec2D_Float size;
    };
    
    struct GameLevel {

        GameLevel(const GameLevelProperties& props, RenderingContext* _context);
        ~GameLevel();

        void render();
        void update();

        void handleEvents( const SDL_Event& event );

        private:

            RenderingContext* context;
            Vec2D_Float worldSize;
            ViewPort viewport;
            SDL_Color backgroundColor;      
            
            std::vector<GameThing*> things;

            PlayerThing* player_thing_p = NULL;
    };
}