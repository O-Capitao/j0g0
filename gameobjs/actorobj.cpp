#include "actorobj.hpp"

using namespace j0g0;

Actor::Actor(
    RenderingContext *cntxt, 
    SpriteSheet *ss, 
    ViewPort *vp,
    const ActorProperties &props
):GameObj( cntxt )
{

    _viewport_p = vp;
    _spriteSheet_p = ss;

    _properties = props;
    _positionInWorld = props.initialPositionInWorld;
    
    _positionInCanvas = _viewport_p->viewPortToCanvas(
        _viewport_p->worldToViewPort( _positionInWorld )
    );

    _boundingBox_InCanvas = {
        .w = _properties.boundingBox_SliceCoordinates.w,
        .h = _properties.boundingBox_SliceCoordinates.h,
        .x = _positionInCanvas.x + _properties.boundingBox_SliceCoordinates.x,
        .y = _positionInCanvas.y + _properties.boundingBox_SliceCoordinates.y - _spriteSheet_p->getSliceSize().y,
    };

    _boundingBox_InWorld = _viewport_p->transformRect_canvasToWorld( _boundingBox_InCanvas );

    _initAnimations( props.spriteAnimations );
    _setActiveAnimation( props.idleAnimationId );

    // _activeAnimation_p = nullptr;
    _currentGroundLine_p = nullptr;
    _IS_GROUNDED = false;

    SDL_Log("Done Constructing Actor Obj '%s'\n", props.id.c_str());

}

Actor::~Actor(){
    _currentGroundLine_p = nullptr;
    _viewport_p = nullptr;
    _spriteSheet_p = nullptr;
    _activeAnimation_p = nullptr;
}

void Actor::render(){

    _spriteSheet_p->renderSlice( _positionInCanvas, _activeAnimation_p->animation_frames[ _animationCounter ]);

    if (_DEBUG_STROKE){

        SDL_SetRenderDrawColor(context->renderer, 18,200,100,150);
        SDL_RenderDrawRect( context->renderer, &_boundingBox_InCanvas );

        // hit box
        SDL_SetRenderDrawColor(context->renderer, 180,150,130,180);

    }

}

void Actor::update(){

    Uint32 time = SDL_GetTicks();
    _updateSprites(time);

}

void Actor::_initAnimations( const std::vector<SpriteAnimationProperties> &a_vec ){

    for (SpriteAnimationProperties item : a_vec){
        SpriteAnimation animation;

        animation.id = item.id;

        // fill out animation frames
        for (int i = 0; i < item.size; i++ ){

            SpriteSlice slice = _spriteSheet_p->getFrameAt( item.sliceIndexes[i] );
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

void Actor::_updateSprites(Uint32 timer_val){

    int dt = timer_val - _lastTick;
    
    SpriteSlice& _active_frame = _activeAnimation_p->animation_frames[_animationCounter];

    _timeSinceLastFrameChange += dt;
    
    if ( _timeSinceLastFrameChange >= _active_frame.duration_ms){
        
        if (_animationCounter == _activeAnimation_p->animation_frames.size() - 1){
            _animationCounter = 0;
        } else {
            _animationCounter++;
        }

        _timeSinceLastFrameChange = 0;
    }

    _lastTick = timer_val;
}

void Actor::_setActiveAnimation( const std::string &key ){
    // TODO - test passing wrong id
    SpriteAnimation& next_animation = _animations[key];

    _activeAnimation_id = key;
    _activeAnimation_p = &next_animation;
}