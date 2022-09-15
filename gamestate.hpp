#pragma once

#include <vector>

#include <SDL.h>

#include "graphics/rendering.hpp"
#include "gameobjs/bitmaptext.hpp"
#include "gamelevel.hpp"

namespace j0g0 {

    enum GameStateEnum{
        STATE_NULL,
        STATE_PAUSE,
        STATE_PLAY,
        STATE_EXIT,
        STATE_GAME_OVER_SCREEN
    };

    class GameState {

        public:
            GameState(RenderingContext* context, SpriteSheetManager *spriteSheetManager);
            virtual ~GameState();
            
            virtual void render() = 0;
            virtual void update() = 0;
            virtual size_t handleEvents() = 0;

            // bool getEndStateRequested(){ return _endStateRequested; }
            bool RESTART_FLAG = false;

        protected:
        
            RenderingContext* _context_p;
            SpriteSheetManager* _spriteSheetManager_p;

            // Allows for EXIT command coming
            // from the inner context - e.g.: Player Dies in PlayState, 
            // Continue timer ends...
            // etc.
            bool _endStateRequested = false;
    };


    class PauseState: public GameState {

        public:

            PauseState(
                RenderingContext* context, 
                SpriteSheetManager *spriteSheetManager
            );
            ~PauseState();

            void render();
            void update();
            size_t handleEvents();

        

        private:
            
            const int optPixelOffsert = 15;
            SDL_Color _background_color;
            BitmapTextLine _headerMessage;
            std::vector<BitmapTextLine> _options;
            
            const std::string _optionTexts[3] = {
                "RESUME",
                "RESTART",
                "QUIT"
            };

            int _activeOptionIndex;

            enum _PauseActionsEnum {
                ACCEPT,
                ESCAPE,
                NAVIGATE_UP,
                NAVIGATE_DOWN,
                IDLE
            };

            void _renderSelectionCarat();
            _PauseActionsEnum _actionKeyMap( SDL_Keycode key );
    };


    class PlayState: public GameState {

        public:

            PlayState(
                RenderingContext* context, 
                SpriteSheetManager *spriteSheetManager,
                const std::string &configFilePath
            );
            ~PlayState(){};
            
            void render();
            void update();
            
            size_t handleEvents();

        private:
            
            GameLevel level;
            
            void _refreshInfoMessage();

            // FOR DEBUG
            int frameCounter;
            int lastDisplay;
            

    };

    class TimedMessageState: public GameState {
        public:
            TimedMessageState(
                RenderingContext* context, 
                SpriteSheetManager *spriteSheetManager,
                std::string &message,
                int durationMs
            );

            void render();
            void update();
            size_t handleEvents();

        private:

            SDL_Color _background_color;
            std::string _message;
            BitmapTextLine _line;
            int _durationMs;
            Uint32 _start_click;
    };

}