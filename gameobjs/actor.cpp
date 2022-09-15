#include "actor.hpp"
#include "game-defines.hpp"

using namespace j0g0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
Actor::Actor(
    RenderingContext *cntxt, 
    SpriteSheet *ss, 
    ViewPort *vp,
    const ActorConfig &props
):GameObj( cntxt ),
_properties(props),
_viewport_p(vp),
_spriteSheet_p(ss),
_offsetWorld({
    .x = _viewport_p->scaleToWorld( _properties.boundingBox_SliceCoordinates.x ),
    .y = _viewport_p->scaleToWorld( _properties.boundingBox_SliceCoordinates.y )
}),
_physicsModel(
    _properties.initialPositionInWorld,
    {   
        // bounding box in the world 
        .x = vp->scaleToWorld( _properties.boundingBox_SliceCoordinates.w ),
        .y = vp->scaleToWorld( _properties.boundingBox_SliceCoordinates.h )
    },
    _properties.initialVelocity,
    {0,-9.8},
    _properties.id  
)
{

    Vec2D_Int sliceSize_canvas  = ss->getSliceSize();

    _spriteSliceSize_world = {
        .x = _viewport_p->scaleToWorld( sliceSize_canvas.x ),
        .y = _viewport_p->scaleToWorld( sliceSize_canvas.y )
    };

    // Sprites
    _initAnimations( props.spriteAnimations );
    _linkAnimations();
    _setActiveAnimation( props.idleAnimationId );

#if DEBUG
    SDL_Log("Done Constructing Actor Obj '%s'\n", props.id.c_str());
#endif

}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
SDL_Rect Actor::_getSliceBB_InCanvas(){
    SDL_FRect worldBB = _physicsModel.getBoundingBox();

    SDL_Rect bb_in_canvas = _viewport_p->transformRect_Viewport2Canvas(
        _viewport_p->transformRect_World2Viewport(
            {
                .x = worldBB.x - _offsetWorld.x,
                .y = worldBB.y - _offsetWorld.y,
                .w = _spriteSliceSize_world.x,
                .h = _spriteSliceSize_world.y
            }
        )
    );

    return bb_in_canvas;
}

void Actor::render(){
    SDL_FRect worldBB = _physicsModel.getBoundingBox();
    SDL_Rect tgt = _getSliceBB_InCanvas();

    _spriteSheet_p->renderSlice( 
        {
            .x = tgt.x, .y = tgt.y
        }, 
        _activeAnimation_p->animation_frames[ _animationCounter ],
        _IS_FLIPPED
    );

    // if (_DEBUG_STROKE){
    #if DEBUG
    SDL_Rect _physBB_Canvas = _viewport_p->transformRect_Viewport2Canvas(
        _viewport_p->transformRect_World2Viewport(worldBB)
    );

    SDL_SetRenderDrawColor(context->renderer, 18,200,100,150);
    SDL_RenderDrawRect( context->renderer, &tgt );

    // hit box
    SDL_SetRenderDrawColor(context->renderer, 180,150,130,180);
    SDL_RenderDrawRect( context->renderer, &_physBB_Canvas );
    #endif
    // }

}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void Actor::update(float dt_s){

    _physicsModel.update(dt_s);

    _updateSprites(floor(dt_s * 1000));
    
    isExpired = !_physicsModel.checkIfInBounds();

}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
ActorModel *Actor::getPhysicsModel_Ptr(){
    return &_physicsModel;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void Actor::_initAnimations( const std::vector<SpriteAnimationConfig> &a_vec ){

    for (SpriteAnimationConfig item : a_vec){

        SpriteAnimation animation = {
            .id = item.id,
            .isTransient = item.isTransient,
            .toId = item.transientTo,
            .to = nullptr
        };

        // fill out animation frames
        for (int i = 0; i < item.size; i++ ){

            SpriteAnimationSlice slice = _spriteSheet_p->getFrameAt( item.sliceIndexes[i] );
            slice.duration_ms = item.sliceDurations_ms[i];
            slice.center_of_rotation = {.x = 0, .y = 0};
            slice.flip = SDL_FLIP_NONE;
            slice.quarter_turns_ccw = 0;
            slice.offset_for_rotation = {.x = 0, .y = 0};

            animation.animation_frames.push_back( slice );

        }

        _animations[item.id] = animation;
    }

}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void Actor::_linkAnimations(){
    for (auto animation : _animations){
        SpriteAnimation a = animation.second;

        if (a.isTransient){
            
            assert(_animations.find(a.toId) != _animations.end());
            a.to = &_animations[ animation.second.toId ];
        
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void Actor::_updateSprites(Uint32 dt){
    
    SpriteAnimationSlice& _active_frame = _activeAnimation_p->animation_frames[_animationCounter];

    if (_active_frame.duration_ms == 0){
        // hold this frame until State changes
        return;
    }

    // _timeSinceLastFrameChange += dt;
    _animationTimer += dt;
    
    if ( _animationTimer >= _active_frame.duration_ms){
        
        if (_animationCounter == _activeAnimation_p->animation_frames.size() - 1){
            
            if (_activeAnimation_p->isTransient){
                _setActiveAnimation(_activeAnimation_p->toId);
            } else {
                _animationCounter = 0;
            }
            
        
        } else {
            _animationCounter++;
        }

        _animationTimer = 0;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void Actor::_setActiveAnimation( const std::string &key ){
    // TODO - test passing wrong id
    #if DEBUG
    printf("Setting Active Animation %s\n", key.c_str());
    #endif

    SpriteAnimation& next_animation = _animations[key];

    _activeAnimation_id = key;
    _activeAnimation_p = &next_animation;
    _animationTimer = 0;
    _animationCounter = 0;
}



// PLAYER ACTOR
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerActor::update(float dt_s){
    
    
    Actor::update(dt_s);

    // some exceptions to my other rules...
    if (!_physicsModel.isItOnGround() && _physicsModel.getVelocity().y < 0 && _activeAnimation_id != "jump-down"){
        _setActiveAnimation("jump-down");
    }

    if (_physicsModel.isItOnGround() && _activeAnimation_id == "jump-down"){
        _land();
    }

    SDL_FRect bb = _physicsModel.getBoundingBox();
    
    if (_activeAction == ActorMovementActions::JUMP){

        if (_physicsModel.isItOnGround()){
            // we landed.
            // TODO - add roll jere
            if (_requestedAction == ActorMovementActions::WALK_LEFT || _requestedAction == ActorMovementActions::WALK_RIGHT ){
                _walk(_requestedAction == ActorMovementActions::WALK_LEFT);
            } else if (_requestedAction == ActorMovementActions::JUMP){
                _jump(_jumpDV);
            } else {
                _land();
            }
        } else {
            // nudge L / R while we fall
            if (_requestedAction == ActorMovementActions::WALK_LEFT || _requestedAction == ActorMovementActions::WALK_RIGHT ){ 
                bb.x += ((_requestedAction == ActorMovementActions::WALK_LEFT) ? - 0.05 : 0.05);
            }
            // undo input
            _requestedAction = ActorMovementActions::NONE;

            
        }
    
    
    
    } else if (_activeAction == ActorMovementActions::WALK_LEFT || _activeAction == ActorMovementActions::WALK_RIGHT ){

        if (!_physicsModel.isItOnGround()){
            // dont apply dv to this jump -> just switch states and animations
            _jump(0);
        } else if ( _requestedAction == ActorMovementActions::JUMP ){
            _jump(_jumpDV);
        } else if ( _requestedAction == ActorMovementActions::NONE ){
            _idle();
        } else if ( (_requestedAction == ActorMovementActions::WALK_LEFT && _activeAction == ActorMovementActions::WALK_RIGHT) 
            || (_requestedAction == ActorMovementActions::WALK_LEFT && _activeAction == ActorMovementActions::WALK_RIGHT)
        ){
            _walk( _requestedAction == ActorMovementActions::WALK_LEFT );
        }
    
    } else {

        if (_requestedAction == ActorMovementActions::JUMP){
            _jump(_jumpDV);
        } else if ( _requestedAction == ActorMovementActions::WALK_LEFT || _requestedAction == ActorMovementActions::WALK_RIGHT ){
            _walk(_requestedAction == ActorMovementActions::WALK_LEFT);
        }
    }
    
    if (bb.y < 0){
        isExpired = true;
    #if DEBUG
        printf("PlayerActor fell!\n");
    #endif
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// Input sets the _requestedAction
void PlayerActor::handleInput( const SDL_KeyboardEvent &input ){

    
    if (input.type == SDL_KEYDOWN && input.keysym.sym == SDLK_SPACE){
        _requestedAction = ActorMovementActions::JUMP;
    }

    else if ( (input.keysym.sym == SDLK_LEFT || input.keysym.sym == SDLK_RIGHT)){

        if (input.type == SDL_KEYDOWN ){

            _requestedAction = (input.keysym.sym == SDLK_LEFT) ?  
                ActorMovementActions::WALK_LEFT : ActorMovementActions::WALK_RIGHT; 

        } else if ( input.type == SDL_KEYUP ){
            _requestedAction = ActorMovementActions::NONE;
        }
    }
    else {
        // go to idle if allowed
        _requestedAction = ActorMovementActions::NONE;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerActor::_jump(float dv){

    if (_physicsModel.isItOnGround()){

        _physicsModel.jump({
            .x=0, 
            .y=_properties.jumpDvY
        });
                
        _activeAction = ActorMovementActions::JUMP;
        _setActiveAnimation("jump-up");

    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerActor::_walk( bool goLeft ){

    if (!_physicsModel.isItOnGround()){
        // Cant walk when im falling.
        return;
    }

    _IS_FLIPPED = goLeft;

    if ( _activeAction != ActorMovementActions::WALK_LEFT && _activeAction != ActorMovementActions::WALK_RIGHT ){
        _physicsModel.setWalkingVelocity( (goLeft ? -1 : 1) * _properties.walkSpeed );
        _setActiveAnimation("walk");
        _activeAction = _requestedAction; // which should be walk -> u dont walk without wanting to.
    }
    
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerActor::_idle(){
    if (_physicsModel.isItOnGround()){
        _setActiveAnimation("idle");
        _physicsModel.setWalkingVelocity(0);
        _activeAction = ActorMovementActions::NONE;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void PlayerActor::_land(){
    if (_physicsModel.isItOnGround()){
        _setActiveAnimation("land");
        _physicsModel.setWalkingVelocity(0);
        _activeAction = ActorMovementActions::NONE;
    }
}