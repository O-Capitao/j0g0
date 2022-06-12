#pragma once

#include "vector"
#include "string"

#include "SDL.h"


#include "graphics/gamegeometry.hpp"
#include "graphics/viewport.hpp"
#include "graphics/spritesheet.hpp"

#include "gameobjs/platform.hpp"
#include "gameconfig.hpp"


namespace j0g0 {
    struct GameLevel {

        GameLevel( 
            std::string levelPropsFile, 
            RenderingContext* _context,
            SpriteSheetManager *ssm
        );

        ~GameLevel();

        void render();
        void update();

        void handleEvents( const SDL_Event& event );

        private:

            RenderingContext* _context_p;
            SpriteSheetManager *_ssm_p;
            GameLevelProperties _properties;
            ViewPort *_viewport_p;
            GameConfigReader _reader;

            std::vector<Platform*> _platforms_p_vec;
    };
}