#pragma once

#include "gameobj.hpp"
#include "gameconfig.hpp"
#include "graphics/spritesheet.hpp"
#include "graphics/viewport.hpp"
#include "physics/physics.hpp"

namespace j0g0 {

    struct Actor: public GameObj {

        Actor( 
            RenderingContext *cntxt, 
            SpriteSheet *ss, 
            ViewPort *vp,
            const ActorProperties &props
        );

        ~Actor();

        void render();
        void update(Uint32 dt);

        BoxPhysicsModel *getPhysicsModel_Ptr();

        private:

            ViewPort *_viewport_p;
            SpriteSheet *_spriteSheet_p;
            SpriteAnimation *_activeAnimation_p;

            ActorProperties _properties;

            // used to draw the sprite slice
            // to the screen
            // Vec2D_Int _spriteSlice_handle_canvas;
            Vec2D_Float _offsetWorld;
            Vec2D_Float _spriteSliceSize_world;

            BoxPhysicsModel _physicsModel;

            std::unordered_map<std::string,SpriteAnimation> _animations;
            std::string _activeAnimation_id;

            bool _IS_GROUNDED = false;
            bool _DEBUG_STROKE = true;

            Uint8 _animationCounter = 0;
            Uint32 _animationTimer = 0;
            
            void _setActiveAnimation( const std::string &key );
            void _initAnimations( const std::vector<SpriteAnimationProperties> &a_vec );
            void _updateSprites( Uint32 time );
            SDL_Rect _getSliceBB_InCanvas();

    };
}
