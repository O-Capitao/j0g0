#include <iostream>
#include <string>

#include "SDL.h"

#include "gamestate.hpp"

using namespace j0g0;




/*
 *  GameState - Helper Methods Implementation 
 */
GameState::GameState(RenderingContext* _context)
:context(_context){
}

GameState::~GameState(){
}







/*
 *  PauseState 
 * 
 */
PauseState::PauseState( RenderingContext* _context )
    :GameState(_context) //,
    // message( BitmapTextThing( _context, 2 ))
{
    background_color = {.r = 255, .g = 107, .b= 151, .a = 255 };

    context->setBackgroundColor(background_color);

    // message.setContent("PAUSE");
    // message.centerOnCanvas();

}

PauseState::~PauseState(){
}

void PauseState::render(){

    context->beginRenderStep();

    // message.render();

    context->endRenderStep();
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
PlayState::PlayState( RenderingContext* _context, GameLevelProperties levelProperties )
    :GameState(_context), level( levelProperties, _context ) //,
    // message( BitmapTextThing( _context, 1 ))
{

    background_color = {.r = 255, .g = 181, .b= 181, .a = 255 };
    context->setBackgroundColor(background_color);
    frameCounter = 0;
    lastDisplay = SDL_GetTicks();

}

PlayState::~PlayState(){
}

void PlayState::render(){
    context->beginRenderStep();
    level.render();
    context->endRenderStep();
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