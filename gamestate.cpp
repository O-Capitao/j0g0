#include <string>

#include "game-defines.hpp"
#include "gamestate.hpp"

using namespace j0g0;



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *  GameState - Helper Methods Implementation 
 */
GameState::GameState(RenderingContext* c, SpriteSheetManager *ssm)
:_context_p(c),
_spriteSheetManager_p(ssm){
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
GameState::~GameState(){
    _context_p = NULL;
    _spriteSheetManager_p = NULL;
}



/*
 *  PauseState 
 * 
 */
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
PauseState::PauseState(RenderingContext* cntxt, SpriteSheetManager *ssm)
:GameState(cntxt,ssm),
_headerMessage( cntxt, ssm->getSpriteSheet("bitmap-text") )
{
    _background_color = {.r = 240, .g = 246, .b= 240, .a = 255 };
    _context_p->setBackgroundColor(_background_color);

    _headerMessage.setContent("PAUSE");
    
    _headerMessage.setPositionInCanvas({
        // x is centered
        .x = (_context_p->config.canvasSize.x - _headerMessage.getContentSize().x)/2,
        // y has an offset of 100 to the top
        .y = (_context_p->config.canvasSize.y - _headerMessage.getContentSize().y)/2 - 50,
    });
    
    int optCounter = 0;
    _activeOptionIndex = 0;
    // remainging options
    for (auto optText : _optionTexts){
        BitmapText opt( cntxt, _spriteSheetManager_p->getSpriteSheet("bitmap-text") );
        opt.setContent(optText);

        opt.setPositionInCanvas({
            .x = (_context_p->config.canvasSize.x - _headerMessage.getContentSize().x)/2,
            .y = (_context_p->config.canvasSize.y - _headerMessage.getContentSize().y)/2 + optPixelOffsert * optCounter++,
        });

        _options.push_back(opt);
    }
    #if DEBUG
    printf("Built PauseState\n");
    #endif

}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
PauseState::~PauseState(){
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void PauseState::render(){

    _context_p->beginRenderStep();
    _context_p->setBackgroundColor(_background_color);
    
    _headerMessage.render();

    for (auto _opt : _options){
        _opt.render();
    }

    _renderSelectionCarat();

    _context_p->endRenderStep();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
size_t PauseState::handleEvents()
{
    SDL_Event e;
    size_t ret_val = GameStateEnum::STATE_PAUSE; 

    while( SDL_PollEvent( &e ) != 0 )
    {
        if (e.type == SDL_QUIT) {
            
            ret_val = GameStateEnum::STATE_EXIT;

        } else if (e.type == SDL_KEYDOWN){

            switch( _actionKeyMap( e.key.keysym.sym ) )
            {
                case (_PauseActionsEnum::ESCAPE):
                    ret_val = GameStateEnum::STATE_PLAY;
                    break;
                case _PauseActionsEnum::NAVIGATE_DOWN:
                    if (_activeOptionIndex == 2){
                        _activeOptionIndex = 0;
                    } else {
                        _activeOptionIndex++;
                    }
                    break;
                case _PauseActionsEnum::NAVIGATE_UP:
                    if (_activeOptionIndex == 0){
                        _activeOptionIndex = 2;
                    } else {
                        _activeOptionIndex--;
                    } 
                    break;
                case _PauseActionsEnum::ACCEPT:
                    switch (_activeOptionIndex)
                    {

                        case 0:
                            
                            ret_val = GameStateEnum::STATE_PLAY;
                            break;
                        case 1:
                            RESTART_FLAG = true;
                            ret_val = GameStateEnum::STATE_PLAY;
                            break;
                        case 2:
                            ret_val = GameStateEnum::STATE_EXIT;
                            break;
                        default:
                            throw std::runtime_error("Invalid Pause State Opt. Bad Progrm State.");
                    }
                    break;
                default:
                    throw std::runtime_error("Invalid Pause Act Opt. Bad Progrm State.");
            } 
        }
    }
    return ret_val;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void PauseState::update(){
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
PauseState::_PauseActionsEnum PauseState::_actionKeyMap( SDL_Keycode key ){

    if ( key == SDLK_RETURN || key == SDLK_RETURN2){
        return _PauseActionsEnum::ACCEPT;
    } else if ( key == SDLK_UP ){
        return _PauseActionsEnum::NAVIGATE_UP;
    } else if ( key == SDLK_DOWN ){
        return _PauseActionsEnum::NAVIGATE_DOWN;
    }

    return _PauseActionsEnum::IDLE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void PauseState::_renderSelectionCarat(){
    // do it very stupid for now
    BitmapText &activeOpt = _options[_activeOptionIndex];
    Vec2D_Int position = activeOpt.getPositionInCanvas();

    SDL_Rect carat = {
        .h = 6,
        .w = 6,
        .x = position.x - 8,
        .y = position.y + 1
    };

    SDL_SetRenderDrawColor(_context_p->renderer, 34,35,35,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawRect( _context_p->renderer, &carat );
}





/*
 *  PlayState
 * 
 */
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// TODO - a central config file path
PlayState::PlayState( RenderingContext* cntxt, SpriteSheetManager *ssm, const std::string &configFilePath )
:GameState(cntxt,ssm ), 
level( configFilePath, cntxt, ssm )
{

    frameCounter = 0;
    lastDisplay = SDL_GetTicks();

}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
PlayState::~PlayState(){
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayState::render(){
    _context_p->beginRenderStep();
    level.render();
    _context_p->endRenderStep();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayState::update(){

    LevelState levelState = level.update();

    if (levelState == LevelState::GAME_OVER){
        _endStateRequested = true;
    }

    #if DEBUG

    int _ellapsedTimeSinceLastDisplay = SDL_GetTicks() - lastDisplay;
    frameCounter++;

    if ( _ellapsedTimeSinceLastDisplay >= 1000 ){
        
        std::string debug_string = "Displaying at " + std::to_string( frameCounter ) + " FPS.\n";
        SDL_Log( "%s", debug_string.c_str() );

        lastDisplay += _ellapsedTimeSinceLastDisplay;
        frameCounter = 0;
    }

    #endif
    
    
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
size_t PlayState::handleEvents(){
    size_t retval = GameStateEnum::STATE_PLAY;
    SDL_Event e;
    while( SDL_PollEvent( &e ) != 0 )
    {
        if (e.type == SDL_QUIT) {
            
            retval = GameStateEnum::STATE_EXIT;

        } else if ( e.type == SDL_KEYDOWN || e.type == SDL_KEYUP ){

            if ( e.key.keysym.sym == SDLK_ESCAPE ){
                retval = GameStateEnum::STATE_PAUSE;
            } else {
                level.handleEvents(e);
            }
        }
    }
    return retval;
}
