#include <iostream>

#include "game.hpp"
#include "gamestate.hpp"

using namespace j0g0;

Game::~Game(){
}

void Game::init(RenderingContext* _context){

    context = _context;
    state = NULL;

    // initial state is PauseState
    stateId = GameStates::STATE_PAUSE;

    // no State Change scheduled until User Input
    nextState = stateId;

    // Init Pause State
    state = new PauseState(context);

    // init level.
    level.size = {.x=10, .y=10};
}

void Game::run(){

    while ( stateId != GameStates::STATE_EXIT )
    {
        state->render();
        state->update();
        nextState = state->handleEvents();
        changeState();
        
    }
}

void Game::changeState(){
    if( nextState != stateId ){
        
        //Delete the current state
        delete state;

        switch (nextState){
            case GameStates::STATE_PAUSE:
                state = new PauseState( context );
                break;
            case GameStates::STATE_PLAY:
                state = new PlayState( context, level );
                break;
        }

        stateId = nextState;
    }
}
