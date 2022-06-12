#include <iostream>

#include "gamelevel.hpp"

using namespace j0g0;

GameLevel::GameLevel( std::string levelPropsFile, RenderingContext* _context, SpriteSheetManager *ssm)
:_context_p(_context),
_ssm_p(ssm)
{

    _properties = _reader.buildGameLevelProperties( levelPropsFile );
    
    ViewPortProperties vp_conf = _reader.generateViewPortProperties( levelPropsFile );
    _viewport_p = new ViewPort( vp_conf, _context->canvasProperties);

    std::vector<PlatformProperties> _platforms_props = _reader.generatePlatformProperties( levelPropsFile );

    for (auto pp : _platforms_props){

        SpriteSheet *_used_ss = _ssm_p->getSpriteSheet(pp.spriteSheetId);
        _platforms_p_vec.push_back( new Platform( _context_p, _used_ss, _viewport_p, pp ) );

    }
    
}

GameLevel::~GameLevel(){

    for (auto p : _platforms_p_vec)
    {
        delete p;
        p = NULL;
    }
}

void GameLevel::render(){

    _context_p->setBackgroundColor(_properties.backgroundColor);
    
    for (Platform *_p: _platforms_p_vec){
        _p->render();
    }
}

void GameLevel::update(){
    // for (int i = 0; i < things.size(); i++){

    //     things[i]->update();

    // }
}

void GameLevel::handleEvents( const SDL_Event& evt ){
    // player_thing_p->handleEvent( evt );
}
