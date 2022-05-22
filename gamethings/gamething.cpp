#include <iostream>

#include "gamething.hpp"

using namespace j0g0;

/*
* Game Thing
*
*/
GameThing::GameThing(RenderingContext* _context, const SDL_Rect& _positionInCanvas)
:boundingBoxInCanvas( _positionInCanvas )
{
    context = _context;
}

GameThing::~GameThing(){
    // std::cout << "\n    ~DESTROY GameThing\n";
}

void GameThing::setBoundingBoxPositionInCanvas( const Vec2D_Int& _position ){
    
    this->boundingBoxInCanvas.x = _position.x;
    this->boundingBoxInCanvas.y = _position.y;
    
}

void GameThing::centerOnCanvas(){

    this->boundingBoxInCanvas.x = ( context->canvasProperties.size.x / 2) - ( boundingBoxInCanvas.w / 2);
    this->boundingBoxInCanvas.y = ( context->canvasProperties.size.y / 2) - ( boundingBoxInCanvas.h / 2);

}












/*
* Sprite Thing
*
*/
SpriteThing::SpriteThing(
        RenderingContext* _context,
        std::string path_to_img,
        int n_frames,
        int x_divisions, 
        int y_divisions,
        const Vec2D_Float& initialPosition,
        ViewPort* _viewport
    )
    :GameThing( _context, {.x=0,.y=0,.w=0,.h=0} ),
    spriteSheet( _context )
{
    spriteSheet.initSpriteSheet( 
        path_to_img, 
        x_divisions, 
        y_divisions );

    boundingBoxInCanvas.w = spriteSheet.step_x;
    boundingBoxInCanvas.h = spriteSheet.step_y;

    if (_viewport != NULL){
        viewPort_p = _viewport;

        boundingBoxInWorld.x = initialPosition.x;
        boundingBoxInWorld.y = initialPosition.y;

        boundingBoxInWorld.w = ((float)boundingBoxInCanvas.w ) / (viewPort_p->pixel_per_meter);
        boundingBoxInWorld.h = ((float)boundingBoxInCanvas.h ) / (viewPort_p->pixel_per_meter);
    }

    time_since_frame_changed = 0;
    last_tick = SDL_GetTicks();
}

SpriteThing::~SpriteThing(){
    // delete viewPort_p;
    viewPort_p = NULL;
};

void SpriteThing::render(){

    SDL_Rect src;
    
    src = active_animation.animation_frames[frame_counter].frame;
    _calculate_CanvasPosition();

    /* casts may be necessary for some compiler settings or languages (e.g. C++) */
    SDL_RendererFlip flip = moveForward ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

    SDL_RenderCopyEx( 
        context->renderer, 
        spriteSheet.texture, 
        &src, &boundingBoxInCanvas,0,NULL,flip
    );

};

void SpriteThing::update(){


    int timer_val = SDL_GetTicks();
    int dt = timer_val - last_tick;
    SpriteFrame& _active_frame = active_animation.animation_frames[frame_counter];

    time_since_frame_changed += dt;
    
    if ( time_since_frame_changed >= _active_frame.duration_ms){
        
        if (frame_counter == active_animation.animation_frames.size() - 1){
            frame_counter = 0;
        } else {
            frame_counter++;
        }

        time_since_frame_changed = 0;
    }

    last_tick = timer_val;

};

void SpriteThing::setActiveAnimation( int animation_id )
{
    active_animation = animations.find(animation_id)->second;
    frame_counter = 0;
}

void SpriteThing::initAnimation( 
    int animation_id, 
    int start_index, 
    int n_frames, 
    int* frame_durations )
{
    SpriteSheetAnimation _current_animation;
    
    for (int i = start_index; i < start_index + n_frames; i++ ){
        _current_animation.animation_frames.push_back(
            spriteSheet.getFrameAt(i)
        );

        _current_animation.animation_frames.back().duration_ms = frame_durations[i];
    }

    animations.insert(
        std::make_pair( animation_id, _current_animation)
    );
}

void SpriteThing::_calculate_CanvasPosition(){
    this->boundingBoxInCanvas = viewPort_p->transformRect_Viewport2Canvas(
        viewPort_p->transformRect_World2Viewport( this->boundingBoxInWorld )
    );
}
