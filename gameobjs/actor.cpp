#include "actor.hpp"
#include "game-defines.hpp"

using namespace j0g0;

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
})
{

    Vec2D_Int sliceSize_canvas  = ss->getSliceSize();

    _spriteSliceSize_world = {
        .x = _viewport_p->scaleToWorld( sliceSize_canvas.x ),
        .y = _viewport_p->scaleToWorld( sliceSize_canvas.y )
    };

    // init Physics:

    // take into account the deltas in props.boundingBox_SliceCoordinates
    FloatRect _bbWorld = {
        .x = _properties.initialPositionInWorld.x,
        .y = _properties.initialPositionInWorld.y,
        .w = _viewport_p->scaleToWorld( _properties.boundingBox_SliceCoordinates.w ),
        .h = _viewport_p->scaleToWorld( _properties.boundingBox_SliceCoordinates.h )
    };

    _physicsModel = BoxPhysicsModel(
        _properties.mass,
        _properties.frictionCoef,
        _properties.initialPositionInWorld,
        _bbWorld,
        _properties.id,
        _properties.initialVelocity, 
        false
    );


    // Sprites
    _initAnimations( props.spriteAnimations );
    _linkAnimations();
    _setActiveAnimation( props.idleAnimationId );

    #if DEBUG
    SDL_Log("Done Constructing Actor Obj '%s'\n", props.id.c_str());
    #endif

}

SDL_Rect Actor::_getSliceBB_InCanvas(){

    SDL_Rect bb_in_canvas = _viewport_p->transformRect_Viewport2Canvas(
        _viewport_p->transformRect_World2Viewport(
            {
                .x = _physicsModel.box.x - _offsetWorld.x,
                .y = _physicsModel.box.y - _offsetWorld.y,
                .w = _spriteSliceSize_world.x,
                .h = _spriteSliceSize_world.y
            }
        )
    );

    return bb_in_canvas;
}

void Actor::render(){

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
        _viewport_p->transformRect_World2Viewport(_physicsModel.box)
    );

    SDL_SetRenderDrawColor(context->renderer, 18,200,100,150);
    SDL_RenderDrawRect( context->renderer, &tgt );

    // hit box
    SDL_SetRenderDrawColor(context->renderer, 180,150,130,180);
    SDL_RenderDrawRect( context->renderer, &_physBB_Canvas );
    #endif
    // }

}

void Actor::update(float dt_s){

    _physicsModel.update(dt_s);

    _updateSprites(floor(dt_s * 1000));
    
    isGone = _physicsModel.checkIfInBounds();

}

BoxPhysicsModel *Actor::getPhysicsModel_Ptr(){
    return &_physicsModel;
}

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

void Actor::_linkAnimations(){
    for (auto animation : _animations){
        SpriteAnimation a = animation.second;

        if (a.isTransient){
            
            assert(_animations.find(a.toId) != _animations.end());
            a.to = &_animations[ animation.second.toId ];
        
        }
    }
}

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





void PlayerActor::update(float dt_s){
    
    
    Actor::update(dt_s);

    // some exceptions to my other rules...
    if (_physicsModel.FALLING && _physicsModel.getVelocity().y < 0 && _activeAnimation_id != "jump-down"){
        _setActiveAnimation("jump-down");
    }

    if (!_physicsModel.FALLING && _activeAnimation_id == "jump-down"){
        _land();
    }
    
    if (_activeAction == ActorMovementActions::JUMP){

        if (!_physicsModel.FALLING){
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
                _physicsModel.box.x += ((_requestedAction == ActorMovementActions::WALK_LEFT) ? - 0.05 : 0.05);
            }
            // undo input
            _requestedAction = ActorMovementActions::NONE;

            
        }
    
    
    
    } else if (_activeAction == ActorMovementActions::WALK_LEFT || _activeAction == ActorMovementActions::WALK_RIGHT ){

        if (_physicsModel.FALLING){
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
    

    #if DEBUG
    if (_physicsModel.box.y < 0){
        printf("PlayerActor fell!\n");
    }
    #endif
}

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

void PlayerActor::_jump(float dv){

    if (!_physicsModel.FALLING){

        _physicsModel.jump({
            .x=0, 
            .y=_properties.jumpDvY
        });
                
        _activeAction = ActorMovementActions::JUMP;
        _setActiveAnimation("jump-up");

    }
}

void PlayerActor::_walk( bool goLeft ){

    if (_physicsModel.FALLING){
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

void PlayerActor::_idle(){
    if (!_physicsModel.FALLING){
        _setActiveAnimation("idle");
        _physicsModel.setWalkingVelocity(0);
        _activeAction = ActorMovementActions::NONE;
    }
}

void PlayerActor::_land(){
    if (!_physicsModel.FALLING){
        _setActiveAnimation("land");
        _physicsModel.setWalkingVelocity(0);
        _activeAction = ActorMovementActions::NONE;
    }
}