#pragma once

#include <vector>

#include <SDL.h>

#include "graphics/rendering.hpp"
// #include "gamethings/gamething.hpp"
#include "gamelevel.hpp"

namespace j0g0 {





    struct GameState {

        public:
            GameState(RenderingContext* _context);
            virtual ~GameState();

            virtual void render() = 0;
            virtual void update() = 0;
            virtual size_t handleEvents() = 0;

        protected:
            SDL_Texture* canvasTexture;

        protected:
        
            RenderingContext* context;
            SDL_Color background_color;
    };






    struct PauseState: public GameState {

        PauseState(RenderingContext* _context);
        ~PauseState();

        // init Sprites and 
        // other assets/ logic exclusive to this State
        // void init();
        void render();
        void update();
        size_t handleEvents();
        
        // SDL_Texture* masterTexture;
        // BitmapTextThing message;
    };



    

    struct PlayState: public GameState {

        PlayState(RenderingContext* _context, GameLevelProperties _levelProps);
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