#include <iostream>

#include "game.hpp"
#include "gamestate.hpp"
#include "graphics/spritesheet.hpp"
#include "gameconfig.hpp"

using namespace j0g0;

Game::~Game(){
}

void Game::init(RenderingContext* _context)
{
    context = _context;
    spriteSheetManager = new SpriteSheetManager();


    GameConfigReader _reader;
    
    _reader.addLevelSpritesToManager(
        "config/test_level.yaml",
        _context,
        spriteSheetManager
    );

    state = NULL;

    // initial state is PauseState
    stateId = GameStates::STATE_PAUSE;

    // no State Change scheduled until User Input
    nextState = stateId;

    // Init Pause State
    state = new PauseState(context,spriteSheetManager);
}

void Game::run(){

    // CAP FRAME RATE TO 60FPS
    Uint32 _start_cycle, _end_cycle, _sleep_time;
    Uint32 _fixed_cycle_duration = 1000 / 60;

    while ( stateId != GameStates::STATE_EXIT )
    {   
        _start_cycle = SDL_GetTicks();
        
        state->render();
        state->update();
        nextState = state->handleEvents();
        changeState();

        _end_cycle = SDL_GetTicks();

        // CAP RATE
        if (_end_cycle - _start_cycle < _fixed_cycle_duration){
            _sleep_time = _fixed_cycle_duration - (_end_cycle - _start_cycle );
            SDL_Delay(_sleep_time);
        }
        
    }
}

void Game::changeState(){
    if( nextState != stateId ){
        
        //Delete the current state
        delete state;

        switch (nextState){
            case GameStates::STATE_PAUSE:
                state = new PauseState( context,spriteSheetManager );
                break;
            case GameStates::STATE_PLAY:
                state = new PlayState( context,spriteSheetManager );
                break;
        }

        stateId = nextState;
    }
}
