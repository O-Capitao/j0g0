#include <iostream>

#include "gamelevel.hpp"

using namespace j0g0;

GameLevel::GameLevel( std::string levelPropsFile, RenderingContext* _context, SpriteSheetManager *ssm)
:_context_p(_context),
_ssm_p(ssm),
_viewport( ViewPort( {.x=0,.y=0}, 10, _context->canvasProperties ) )
{

    _properties = _reader.buildGameLevelProperties( levelPropsFile );
    
    _reader.addLevelSpritesToManager(
        levelPropsFile,
        _context_p,
        _ssm_p
    );

    std::vector<PlatformProperties> _platforms_props = _reader.generatePlatformProperties( levelPropsFile );

    for (auto pp : _platforms_props){

        SpriteSheet *_used_ss = _ssm_p->getSpriteSheet(pp.spriteSheetId);
        _platforms_p_vec.push_back( new Platform( _context_p, _used_ss, &_viewport, pp ) );

    }

    std::cout << "Done Generating Level!\n";

    // do init with Properties
}

GameLevel::~GameLevel(){

    // for (auto p : things)
    // {
    //     delete p;
    //     p = NULL;
    // } 

    // things.clear();
}

void GameLevel::render(){

    _context_p->setBackgroundColor(_properties.backgroundColor);
    
    // for (int i = 0; i < things.size(); i++){
        
    //     things[i]->render();

    // }

}

void GameLevel::update(){
    // for (int i = 0; i < things.size(); i++){

    //     things[i]->update();

    // }
}

void GameLevel::handleEvents( const SDL_Event& evt ){
    // player_thing_p->handleEvent( evt );
}
