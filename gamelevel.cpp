#include "game-defines.hpp"
#include "gamelevel.hpp"

using namespace j0g0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
GameLevel::GameLevel( std::string levelPropsFile, RenderingContext* _context, SpriteSheetManager *ssm)
:_context_p(_context),
_ssm_p(ssm)
{

    _properties = _reader.readGameLevelConfig( levelPropsFile );
    
    ViewPortConfig vp_conf = _reader.readViewPortConfig( levelPropsFile );
    _viewport_p = new ViewPort( vp_conf, _context->config);

    std::vector<PlatformConfig> _platforms_props = _reader.readPlatformConfig( levelPropsFile );

    for (auto pp : _platforms_props){

        SpriteSheet *_used_ss = _ssm_p->getSpriteSheet(pp.spriteSheetId);


        if (pp.isMovingPlatform){

            _platforms_p_vec.push_back( 
                new MovingRectPlatform( _context_p, _used_ss, _viewport_p, pp ) 
            );
            
            _physicsModel.platforms.push_back(  _platforms_p_vec.back()->getPhysicsModel_ptr() );

        }else {
            _platforms_p_vec.push_back( 
                new RectPlatform( _context_p, _used_ss, _viewport_p, pp ) 
            );

            _physicsModel.platforms.push_back(  _platforms_p_vec.back()->getPhysicsModel_ptr() );
        }

 
    }
    // Init players
    _player_p = nullptr;
    std::vector<ActorConfig> _actors_in = _reader.readActorConfig( levelPropsFile );

    for (auto ap: _actors_in){
        SpriteSheet *_used_ss = _ssm_p->getSpriteSheet(ap.spriteSheetId);

        if (!ap.isPlayer){
            
            _actors_p_vec.push_back( new Actor( _context_p, _used_ss, _viewport_p, ap ) );
            _physicsModel.objects.push_back( _actors_p_vec.back()->getPhysicsModel_Ptr() );
        
        } else {

            assert(_player_p == nullptr);
        
            // init Player only once
            _player_p = new PlayerActor( _context_p, _used_ss, _viewport_p, ap  );
            _physicsModel.objects.push_back( _player_p->getPhysicsModel_Ptr() );

        }
    }

    std::vector<BackgroundConfig> _background_props = _reader.readBackgroundConfig( levelPropsFile );

    for (auto bp : _background_props){
        SpriteSheet *_used_ss = _ssm_p->getSpriteSheet(bp.spriteSheetId);
        _backgrounds_p_vec.push_back( new BackgroundObj(_context_p, _used_ss, _viewport_p, bp) );
    }

    std::vector<PropConfig> _propConfigs = _reader.readPropConfig( levelPropsFile );

    for (auto pc: _propConfigs){
        SpriteSheet *_used_ss = _ssm_p->getSpriteSheet(pc.spriteSheetId);
        _props_p_vec.push_back( new Prop( _context_p, _used_ss, _viewport_p, pc ) );
    }

    _lastTick = 0; // no update cycle was run just yet
    
    #if DEBUG
    SDL_Log("Done Constructing GameLevel.\n");
    #endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
GameLevel::~GameLevel(){

    for (auto a: _platforms_p_vec){
        delete a;
    }
    _platforms_p_vec.empty();

    for (auto b: _backgrounds_p_vec){
        delete b;
    }
    _backgrounds_p_vec.empty();

    for (auto a: _actors_p_vec){
        delete a;
    }
    _actors_p_vec.empty();

    for (auto a:_props_p_vec){
        delete a;
    }
    _props_p_vec.empty();

    delete _player_p;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLevel::render(){
    // _context_p->setBackgroundColor(_properties.backgroundColor);

    for (BackgroundObj *bo_p: _backgrounds_p_vec){
        bo_p->render();
    }

    for (RectPlatform *_p: _platforms_p_vec){
        _p->render();
    }

#if DEBUG

    SDL_SetRenderDrawColor(_context_p->renderer, 18,200,100,50);

    // try plot all world coordinates between the
    // test if start of the viewpport and the end of the vp.
    float ppm = _viewport_p->pixel_per_meter;

    for (float x = 0 ; x <= _properties.worldSize.x; x++ ){

        if ( x > _viewport_p->positionInWorld.x && x < _viewport_p->positionInWorld.x + _viewport_p->sizeInWorld.x){

            Vec2D_Int x_inCanvas = _viewport_p->viewPortToCanvas(
                _viewport_p->worldToViewPort({x,0})
            );

            SDL_RenderDrawLine(
                _context_p->renderer, 
                x_inCanvas.x, 
                0,
                x_inCanvas.x, 
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

#endif

    for (Prop *pr: _props_p_vec){
        pr -> render();
    }

    for (Actor *_a: _actors_p_vec){
        _a->render();
    }

    _player_p->render();
    

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLevel::update(){
    
    if (_lastTick == 0){
        _lastTick = SDL_GetTicks();
        return;
    }

    // lets cleanup before doing any work:
    for ( auto it = _actors_p_vec.begin(); it != _actors_p_vec.end(); ){
        if ((*it)->isGone){

            #if DEBUG
            printf("Killing Actor: %s\n", (*it)->getId().c_str());
            #endif

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

    float dt_s = ((float)dt)*0.001;
    
    // if (_player_p != nullptr){
    _player_p->update(dt_s);
    // }

    for (Actor *_a: _actors_p_vec){
        _a->update(dt_s);
        
    }

    for (RectPlatform* p : _platforms_p_vec){
        p->update(dt_s);
    }

    for (BackgroundObj *bo: _backgrounds_p_vec){
        bo->update(dt_s);
    }

    _physicsModel.resolveModel(dt_s);

    // Update the viewport's position
    // for now, we leave the player always in the middle
    if (_player_p->getPhysicsModel_Ptr()->box.x > (_viewport_p->sizeInWorld.x / 2)){
        _viewport_p->positionInWorld.x = _player_p->getPhysicsModel_Ptr()->box.x - (_viewport_p->sizeInWorld.x / 2);
    } else {
        _viewport_p->positionInWorld.x = 0;
    }
    
    if (_player_p->getPhysicsModel_Ptr()->box.y > (_viewport_p->sizeInWorld.y / 2)){
        _viewport_p->positionInWorld.y = _player_p->getPhysicsModel_Ptr()->box.y - (_viewport_p->sizeInWorld.y / 2);
    } else {
        _viewport_p->positionInWorld.y = 0;
    }

    _lastTick = _now;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLevel::handleEvents( const SDL_Event& evt ){
    // player_thing_p->handleEvent( evt );
    _player_p->handleInput( evt.key );
}

