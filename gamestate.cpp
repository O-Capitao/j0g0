#include <iostream>
#include <string>

#include "SDL.h"

#include "gamestate.hpp"

using namespace j0g0;




/*
 *  GameState - Helper Methods Implementation 
 */
GameState::GameState(RenderingContext* c, SpriteSheetManager *ssm)
:_context_p(c),
_spriteSheetManager_p(ssm){}


GameState::~GameState(){
    _context_p = NULL;
    _spriteSheetManager_p = NULL;
}






/*
 *  PauseState 
 * 
 */
PauseState::PauseState(RenderingContext* cntxt, SpriteSheetManager *ssm)
:GameState(cntxt,ssm)
{
    background_color = {.r = 255, .g = 107, .b= 151, .a = 255 };

    _context_p->setBackgroundColor(background_color);
}

PauseState::~PauseState(){
}

void PauseState::render(){

    _context_p->beginRenderStep();

    // message.render();

    _context_p->endRenderStep();
}

size_t PauseState::handleEvents()
{
    SDL_Event e;
    size_t ret_val = GameStates::STATE_PAUSE; 

    while( SDL_PollEvent( &e ) != 0 )
    {
        if (e.type == SDL_QUIT) {
            
            ret_val = GameStates::STATE_EXIT;

        } else if (e.type == SDL_KEYDOWN){
            switch( e.key.keysym.sym )
            {
                case SDLK_SPACE:
                ret_val = GameStates::STATE_PLAY;
                break;
            }
        }
    }
    return ret_val;
}

void PauseState::update(){
}






/*
 *  PlayState
 * 
 */
PlayState::PlayState( RenderingContext* cntxt, SpriteSheetManager *ssm )
:GameState(cntxt,ssm ), 
level( "config/test_level.yaml", cntxt, ssm )
{

    frameCounter = 0;
    lastDisplay = SDL_GetTicks();

}

PlayState::~PlayState(){
}

void PlayState::render(){
    _context_p->beginRenderStep();
    level.render();
    _context_p->endRenderStep();
}

void PlayState::update(){

    level.update();

    if (debugEnabled){

        int _ellapsedTimeSinceLastDisplay = SDL_GetTicks() - lastDisplay;
        frameCounter++;

        if ( _ellapsedTimeSinceLastDisplay >= 1000 ){
            
            std::string debug_string = "Displaying at " + std::to_string( frameCounter ) + " FPS.\n";
            SDL_Log( "%s", debug_string.c_str() );

            lastDisplay += _ellapsedTimeSinceLastDisplay;
            frameCounter = 0;
        }

    }
    
}

size_t PlayState::handleEvents(){
    size_t retval = GameStates::STATE_PLAY;
    SDL_Event e;
    while( SDL_PollEvent( &e ) != 0 )
    {
        if (e.type == SDL_QUIT) {
            
            retval = GameStates::STATE_EXIT;

        } else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP ){

            if ( e.key.keysym.sym == SDLK_ESCAPE ){
                retval = GameStates::STATE_PAUSE;
            } else {
                level.handleEvents(e);
            }
        }
    }
    return retval;
}