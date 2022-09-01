#pragma once

#include "graphics/rendering.hpp"
namespace j0g0 {

    struct GameObj {

        GameObj(
            RenderingContext* _context
        ):context(_context){};

        virtual ~GameObj(){}

        virtual void render() = 0;
        virtual void update( float dt_s ) = 0;

        // can be queried by anyone to make decisions
        bool isExpired = false;

        protected:
    
            RenderingContext* context;

    };
}
