#pragma once

#include "gamestate.hpp"
#include "graphics/rendering.hpp"
#include "graphics/spritesheet.hpp"

namespace j0g0 {

    struct Game {

        int stateId, nextState;
        RenderingContext* context;
        SpriteSheetManager *spriteSheetManager;
        GameState* state;
        

        // TODO - make this an array or Collection
        GameWorldConfig level;
        
        ~Game();

        void init(RenderingContext* _context, std::string path_to_config);
        void run();
        void changeState();

        private:
            // keep a persistent ptr to a PlayState.
            PlayState* _playState = nullptr;
            
            void _cleanUp();
            void _initSpriteSheets();

            std::string _pathToConfig;
            
    };

}
