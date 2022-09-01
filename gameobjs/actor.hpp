#pragma once

#include "string.h"
#include "gameobj.hpp"
#include "gameconfig.hpp"
#include "graphics/spritesheet.hpp"
#include "graphics/viewport.hpp"
#include "physics/physics.hpp"

namespace j0g0 {

    // Actions that can be requested
    // via User Input in Player Controlled Actor
    // or Programmed Behavior for Drones. 
    enum ActorMovementActions {
        NONE,
        WALK_LEFT,
        WALK_RIGHT,
        JUMP
    };

    class Actor: public GameObj {

        public:

            Actor( 
                RenderingContext *cntxt, 
                SpriteSheet *ss, 
                ViewPort *vp,
                const ActorConfig &props
            );

            void render();
            void update(float dt_s);

            const std::string &getId(){return _properties.id;}
            BoxPhysicsModel *getPhysicsModel_Ptr();

        protected:

            ActorConfig _properties;
            BoxPhysicsModel _physicsModel;

            std::unordered_map<std::string,SpriteAnimation> _animations;
            std::string _activeAnimation_id;

            ActorMovementActions _activeAction;
            ActorMovementActions _requestedAction;

            bool _IS_FLIPPED = false;

            void _setActiveAnimation( const std::string &key );

            SDL_Rect _getSliceBB_InCanvas();
            
        private:
            
            ViewPort *_viewport_p;
            SpriteSheet *_spriteSheet_p;
            SpriteAnimation *_activeAnimation_p;

            Vec2D_Float _offsetWorld;
            Vec2D_Float _spriteSliceSize_world;

            Uint8 _animationCounter = 0;
            Uint32 _animationTimer = 0;

            void _initAnimations( const std::vector<SpriteAnimationConfig> &a_vec );
            void _linkAnimations();
            void _updateSprites( Uint32 time );
    };

    class PlayerActor : public Actor {
        
        public:

            PlayerActor( 
                RenderingContext *cntxt, 
                SpriteSheet *ss, 
                ViewPort *vp,
                const ActorConfig &props
            ):Actor(cntxt, ss, vp, props){};

            void update(float dt_s);

            void handleInput( const SDL_KeyboardEvent &input );

        private:
            // give the player a upward velocity component
            // if the player is currently standiong on a platform
            void _jump(float dv);

            // walk along the current platform
            void _walk(bool headLeft);

            void _idle();

            void _land();

            const float _jumpDV = 4;

    };
}
