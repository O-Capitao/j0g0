#include "game-defines.hpp"
#include "game.hpp"
#include "gamestate.hpp"
#include "graphics/spritesheet.hpp"
#include "gameconfig.hpp"

using namespace j0g0;

Game::~Game(){
    state = nullptr;
    _playState = nullptr;
}

void Game::init(RenderingContext* _context, std::string path_to_config)
{

    #if DEBUG
        printf("Debug Mode Activated.");
    #endif

    context = _context;
    spriteSheetManager = new SpriteSheetManager();
    _pathToConfig = path_to_config;

    GameConfigReader _reader;
    
    std::vector<SpriteSheetConfig> ssc = _reader.readSpriteSheetConfig( path_to_config );

    for (auto ssc_entry : ssc){
        spriteSheetManager->insertSpriteSheet(
            ssc_entry.id,
            context,
            ssc_entry.path,
            ssc_entry.sliceSize,
            ssc_entry.scaleFactor
        );
    }

    state = nullptr;
    _playState = nullptr;

    // initial state is PauseState
    stateId = GameStateEnum::STATE_PAUSE;

    // no State Change scheduled until User Input
    nextState = stateId;

    // Init States
    state = new PauseState(context,spriteSheetManager);
    _playState = new PlayState( context,spriteSheetManager, path_to_config );
}

void Game::run(){

    // CAP FRAME RATE TO 60FPS
    Uint32 _start_cycle, _end_cycle, _sleep_time;
    Uint32 _fixed_cycle_duration = 1000 / 60;

    while ( stateId != GameStateEnum::STATE_EXIT )
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
            case GameStateEnum::STATE_PAUSE: {
                state = new PauseState( context,spriteSheetManager );
                break;
            }
            case GameStateEnum::STATE_PLAY: {
            
                // delete (current) pause state 
                bool restartRequested = state->RESTART_FLAG;
                delete state;

                if (restartRequested){
                    delete _playState;
                    // _playState = NULL;
                    _playState = new PlayState( context,spriteSheetManager, _pathToConfig );
                    
                }
                
                state = _playState;
                break;
            }
            case GameStateEnum::STATE_GAME_OVER_SCREEN: {

                std::string caption = "GAME OVER";

                state = new TimedMessageState(
                    context,
                    spriteSheetManager,
                    caption,
                    5000
                );
            
                break;
            }
        }

        stateId = nextState;
    }
}
