#pragma once

#include "vector"
#include "SDL.h"

#include "graphics/viewport.hpp"
#include "graphics/spritesheet.hpp"
#include "physics/physics.hpp"
#include "gameobjs/platform.hpp"
#include "gameobjs/backgroundobj.hpp"
#include "gameobjs/actor.hpp"
#include "gameobjs/prop.hpp"
#include "gameconfig.hpp"


namespace j0g0 {

    enum LevelState {
        CONTINUE,
        PAUSE,
        GAME_OVER
    };

    class GameLevel {
        public:

            GameLevel( 
                std::string levelPropsFile, 
                RenderingContext* _context,
                SpriteSheetManager *ssm
            );

            ~GameLevel();

            void render();
            LevelState update();
            void handleEvents( const SDL_Event& event );

        private:

            const bool CONSTANT_STEP_MODE = true;

            RenderingContext* _context_p;
            SpriteSheetManager *_ssm_p;
            GameWorldConfig _properties;
            ViewPort *_viewport_p;
            GameConfigReader _reader;

            Uint32 _lastTick;

            WorldModel _physicsModel;

            std::vector<BackgroundObj*> _backgrounds_p_vec;
            std::vector<RectPlatform*> _platforms_p_vec;
            std::vector<Actor*> _actors_p_vec;
            std::vector<Prop*> _props_p_vec;

            PlayerActor *_player_p;
    };
}