#pragma once

#include <vector>

#include <SDL.h>

#include "graphics/rendering.hpp"
#include "gameobjs/bitmaptext.hpp"
#include "gamelevel.hpp"

namespace j0g0 {





    struct GameState {

        public:
            GameState(RenderingContext* context, SpriteSheetManager *spriteSheetManager);
            virtual ~GameState();

            virtual void render() = 0;
            virtual void update() = 0;
            virtual size_t handleEvents() = 0;

        protected:
        
            RenderingContext* _context_p;
            SpriteSheetManager* _spriteSheetManager_p;
    };






    struct PauseState: public GameState {

        PauseState(RenderingContext* context, SpriteSheetManager *spriteSheetManager);
        ~PauseState();

        void render();
        void update();
        size_t handleEvents();
        
        private:
            SDL_Color background_color;
            BitmapText message;
    };



    

    struct PlayState: public GameState {

        PlayState(RenderingContext* context, SpriteSheetManager *spriteSheetManager);
        ~PlayState();
        
        void render();
        void update();
        size_t handleEvents();

        private:
            
            GameLevel level;
            void _refreshInfoMessage();

            // FOR DEBUG
            bool debugEnabled = true;
            // BitmapTextThing message;
            int frameCounter;
            int lastDisplay;
    };

    enum GameStates{
        STATE_NULL,
        STATE_PAUSE,
        STATE_PLAY,
        STATE_EXIT
    };

}