#pragma once

#include "gamestate.hpp"
#include "graphics/rendering.hpp"

namespace j0g0 {

    struct Game {

        int stateId, nextState;
        RenderingContext* context;
        GameState* state;

        // TODO - make this an array or Collection
        GameLevelProperties level;

        ~Game();

        void init(RenderingContext* _context);
        void run();
        void changeState();

        private:
            void cleanUp();
    };

}
