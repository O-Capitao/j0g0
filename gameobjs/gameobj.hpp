#pragma once

#include "graphics/rendering.hpp"

namespace j0g0 {

    struct GameObj {

        GameObj(
            RenderingContext* _context
        ):context(_context){};

        virtual ~GameObj(){ context = NULL; }

        virtual void render() = 0;
        virtual void update() = 0;
        
        protected:
    
            RenderingContext* context;

    };
}
