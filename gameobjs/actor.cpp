#include "actor.hpp"

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

    _offsetWorld = {
        .x = _viewport_p->scaleToWorld( _properties.boundingBox_SliceCoordinates.x ),
        .y = _viewport_p->scaleToWorld( _properties.boundingBox_SliceCoordinates.y )
    };

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
        _bbWorld
    );


    // Sprites
    _initAnimations( props.spriteAnimations );
    _setActiveAnimation( props.idleAnimationId );

    _IS_GROUNDED = false;

    SDL_Log("Done Constructing Actor Obj '%s'\n", props.id.c_str());

}

Actor::~Actor(){
    _viewport_p = nullptr;
    _spriteSheet_p = nullptr;
    _activeAnimation_p = nullptr;
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
        _activeAnimation_p->animation_frames[ _animationCounter ]
    );

    if (_DEBUG_STROKE){

        SDL_Rect _physBB_Canvas = _viewport_p->transformRect_Viewport2Canvas(
            _viewport_p->transformRect_World2Viewport(_physicsModel.box)
        );

        SDL_SetRenderDrawColor(context->renderer, 18,200,100,150);
        SDL_RenderDrawRect( context->renderer, &tgt );

        // hit box
        SDL_SetRenderDrawColor(context->renderer, 180,150,130,180);
        SDL_RenderDrawRect( context->renderer, &_physBB_Canvas );

    }

}

void Actor::update(Uint32 dt){

    _physicsModel.update((float)dt*0.001);
    _updateSprites(dt);

}

BoxPhysicsModel *Actor::getPhysicsModel_Ptr(){
    return &_physicsModel;
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

void Actor::_updateSprites(Uint32 dt){
    
    SpriteSlice& _active_frame = _activeAnimation_p->animation_frames[_animationCounter];

    // _timeSinceLastFrameChange += dt;
    _animationTimer += dt;
    
    if ( _animationTimer >= _active_frame.duration_ms){
        
        if (_animationCounter == _activeAnimation_p->animation_frames.size() - 1){
            _animationCounter = 0;
        } else {
            _animationCounter++;
        }

        _animationTimer = 0;
    }
}

void Actor::_setActiveAnimation( const std::string &key ){
    // TODO - test passing wrong id
    SpriteAnimation& next_animation = _animations[key];

    _activeAnimation_id = key;
    _activeAnimation_p = &next_animation;
}