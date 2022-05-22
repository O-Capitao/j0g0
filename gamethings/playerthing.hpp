#pragma once

#include "gamething.hpp"
#include "collision.hpp"
#include "graphics/viewport.hpp"

namespace j0g0{

    enum PlayerStateEnum {
        IDLE = 0,
        WALKING = 1,
        TURNING = 2,
        FALLING = 3,
        JUMPING = 4
    };

    struct PlayerThing: public SpriteThing {

        PlayerThing(
            RenderingContext* _context,
            std::string path_to_img,
            int n_frames,
            int x_divisions, 
            int y_divisions,            
            Vec2D_Float initialPosition,
            ViewPort* _viewport
        );

        ~PlayerThing();

        void update();
        void render();

        void handleEvent( const SDL_Event& e );

        void initState(PlayerStateEnum stateIndex, SpriteSheetAnimation animation);
        void setActiveState(PlayerStateEnum stateIndex);

        CollisionBox box;
        BoxOffset boxOffset;

        private:
        
            PlayerStateEnum activeState;     
            bool debugStroke = true;
            bool airbourne = true;
            bool directionKeysPressed = false;
            SpriteSheetAnimation states[5];
            Vec2D_Float velocity;
    };
}