#include <iostream>


#include "gamelevel.hpp"

using namespace j0g0;

GameLevel::GameLevel( std::string levelPropsFile, RenderingContext* _context, SpriteSheetManager *ssm)
:_context_p(_context),
_ssm_p(ssm)
{

    _properties = _reader.readGameLevelProperties( levelPropsFile );
    
    ViewPortProperties vp_conf = _reader.readViewPortProperties( levelPropsFile );
    _viewport_p = new ViewPort( vp_conf, _context->canvasProperties);

    std::vector<PlatformProperties> _platforms_props = _reader.readPlatformProperties( levelPropsFile );

    for (auto pp : _platforms_props){

        SpriteSheet *_used_ss = _ssm_p->getSpriteSheet(pp.spriteSheetId);


        if (pp.isMovingPlatform){

            _platforms_p_vec.push_back( 
                new MovingRectPlatform( _context_p, _used_ss, _viewport_p, pp ) 
            );

        }else {
            _platforms_p_vec.push_back( 
                new RectPlatform( _context_p, _used_ss, _viewport_p, pp ) 
            );

            _physicsModel.platforms.push_back(  _platforms_p_vec.back()->getPhysicsModel_ptr() );
        }

 
    }

    std::vector<ActorProperties> _actors_in = _reader.readActorProperties( levelPropsFile );

    for (auto ap: _actors_in){
        SpriteSheet *_used_ss = _ssm_p->getSpriteSheet(ap.spriteSheetId);
        _actors_p_vec.push_back( new Actor( _context_p, _used_ss, _viewport_p, ap ) );
        _physicsModel.objects.push_back( _actors_p_vec.back()->getPhysicsModel_Ptr() );
    }

    _lastTick = 0; // no update cycle was run just yet
    
    SDL_Log("Done Constructing GameLevel.\n");
    
}

GameLevel::~GameLevel(){

    for (auto a: _platforms_p_vec){
        delete a;
    }
    _platforms_p_vec.empty();

    for (auto a: _actors_p_vec){
        delete a;
    }
    _actors_p_vec.empty();
}

void GameLevel::render(){
    _context_p->setBackgroundColor(_properties.backgroundColor);


    for (RectPlatform *_p: _platforms_p_vec){
        _p->render();
    }

    if (_DEBUG_STROKE){

        SDL_SetRenderDrawColor(_context_p->renderer, 18,200,100,50);

        // try plot all world coordinates between the
        // test if start of the viewpport and the end of the vp.
        float ppm = _viewport_p->pixel_per_meter;

        for (float x = 0 ; x <= _properties.worldSize.x; x++ ){

            if ( x > _viewport_p->positionInWorld.x && x < _viewport_p->positionInWorld.x + _viewport_p->sizeInWorld.x){
                SDL_RenderDrawLine(
                    _context_p->renderer, 
                    x * ppm , 
                    0,
                    x * ppm, 
                    _viewport_p->canvasSize.y    
                );
            }
        }

                    
        for ( float y = 0; y <= _properties.worldSize.y; y++){
            if (y > _viewport_p->positionInWorld.y && y < _viewport_p->positionInWorld.y + _viewport_p->sizeInWorld.y){

                Vec2D_Float aux_p = {
                    .x = 0,
                    .y = y
                };

                Vec2D_Int aux_p_in_canvas = _viewport_p->viewPortToCanvas(
                    _viewport_p->worldToViewPort( aux_p )
                );

                SDL_RenderDrawLine(
                    _context_p->renderer,
                    0,
                    aux_p_in_canvas.y,
                    _viewport_p->canvasSize.x,
                    aux_p_in_canvas.y
                );

            }
        }
    }

    for (Actor *_a: _actors_p_vec){
        _a->render();
    }

}

void GameLevel::update(){
    
    if (_lastTick == 0){
        _lastTick = SDL_GetTicks();
        return;
    }

    // lets cleanup before doing any work:
    for ( auto it = _actors_p_vec.begin(); it != _actors_p_vec.end(); ){
        if ((*it)->isGone){

           _physicsModel.removeObject( (*it)->getId() );
           delete * it;
           it = _actors_p_vec.erase(it); 
        
        } else{
            ++it;
        }
    }

    Uint32 _now, dt;
    if (CONSTANT_STEP_MODE){

        dt = 17;
        _now = _lastTick + dt;

    }else {
        _now = SDL_GetTicks();
        dt = _now - _lastTick;
    }


    for (Actor *_a: _actors_p_vec){
        _a->update(dt);
        
    }

    for (RectPlatform* p : _platforms_p_vec){
        p->update(dt);
    }

    _physicsModel.resolveModel(dt);

    _lastTick = _now;
}

void GameLevel::handleEvents( const SDL_Event& evt ){
    // player_thing_p->handleEvent( evt );
}

