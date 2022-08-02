#include <iostream>

#include "game.hpp"
#include "gamestate.hpp"
#include "graphics/spritesheet.hpp"
#include "gameconfig.hpp"

using namespace j0g0;

Game::~Game(){
    state = nullptr;
    _playState = nullptr;
}

void Game::init(RenderingContext* _context)
{
    context = _context;
    spriteSheetManager = new SpriteSheetManager();


    GameConfigReader _reader;
    
    _reader.addLevelSpritesToManager(
        // "config/test_level.yaml",
        "config/space_adv_l0.yaml",
        _context,
        spriteSheetManager
    );

    state = nullptr;
    _playState = nullptr;

    // initial state is PauseState
    stateId = GameStates::STATE_PAUSE;

    // no State Change scheduled until User Input
    nextState = stateId;

    // Init States
    state = new PauseState(context,spriteSheetManager);
    _playState = new PlayState( context,spriteSheetManager );
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
        
        switch (nextState){
            case GameStates::STATE_PAUSE:
                state = new PauseState( context,spriteSheetManager );
                break;
            case GameStates::STATE_PLAY:
            
                // delete (current) pause state 
                bool restartRequested = state->RESTART_FLAG;
                delete state;

                if (restartRequested){
                    delete _playState;
                    // _playState = NULL;
                    _playState = new PlayState( context,spriteSheetManager );
                    
                }
                
                state = _playState;
                
                break;
        }

        stateId = nextState;
    }
}
