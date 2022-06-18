#pragma once

#include "gameobj.hpp"
#include "gameconfig.hpp"
#include "graphics/spritesheet.hpp"
#include "graphics/viewport.hpp"

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
        void update();

        private:

            ViewPort *_viewport_p;
            SpriteSheet *_spriteSheet_p;
            SpriteAnimation *_activeAnimation_p;

            ActorProperties _properties;
            SDL_Rect _boundingBox_InCanvas;
            FloatRect _boundingBox_InWorld;
            
            std::unordered_map<std::string,SpriteAnimation> _animations;
            std::string _activeAnimation_id;
            
            void _setActiveAnimation( const std::string &key );
            // void _initActor( const ActorProperties &actor_properties );
            void _initAnimations( const std::vector<SpriteAnimationProperties> &a_vec );
            void _updateSprites( Uint32 time );

            FloatLine* _currentGroundLine_p;

            Vec2D_Float _positionInWorld;
            Vec2D_Int _positionInCanvas;

            bool _IS_GROUNDED = true;
            bool _DEBUG_STROKE = true;

            Uint8 _animationCounter = 0;
            Uint32 _timeSinceLastFrameChange = 0;
            Uint32 _lastTick;

    };
}
