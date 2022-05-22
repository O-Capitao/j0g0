#include <exception>
#include <cassert>

#include "playerthing.hpp"

using namespace j0g0;

PlayerThing::PlayerThing(
    RenderingContext* _context,
    std::string path_to_img,
    int n_frames,
    int x_divisions, 
    int y_divisions,
    Vec2D_Float initialPosition,
    ViewPort* vp
)
:SpriteThing( _context, path_to_img, n_frames, x_divisions, y_divisions, initialPosition, vp ),
box(CollisionBox()){
    
    // configure BB and physics
    boxOffset = {
        .pxOffsetDown = 0,
        .pxOffsetUp = 14,
        .pxOffsetLeft = 10,
        .pxOffsetRight = 10
    };

    velocity = {
        .x = 0, .y = 0
    };

    // INIT ANIMATIONS
    // ---------------
    // inside here it's ok to have intimate knowledge of
    // the sprite stuff.
    int durations[12];

    for (int i = 0; i<12;i++){
        durations[i] = 100;
    }

    // IDLE
    initAnimation(
        PlayerStateEnum::IDLE,
        11,
        1,
        durations
    );

    //WALKING
    initAnimation(
        PlayerStateEnum::WALKING,
        0,
        12,
        durations
    );

    // TURNING
    initAnimation(
        PlayerStateEnum::TURNING,
        13,8,
        durations
    );

    // FALLING
    initAnimation(
        PlayerStateEnum::FALLING,
        20, 1, 
        durations
    );

    // JUMPING
    initAnimation(
        PlayerStateEnum::JUMPING,
        20, 1, 
        durations
    );

    setActiveState( PlayerStateEnum::FALLING );
}

PlayerThing::~PlayerThing(){
}

void PlayerThing::update(){

    if ( (activeState == PlayerStateEnum::FALLING) || (activeState == PlayerStateEnum::JUMPING) ){

        float g = 9.8;

        int dt = SDL_GetTicks() - last_tick;

        velocity.y += -g * 0.001 * (float)dt;
        
        boundingBoxInWorld.x += velocity.x * 0.001 * (float)dt;
        boundingBoxInWorld.y += velocity.y * 0.001 * (float)dt;

        // only state change here can be to land or die
        // dummy for now
        if (boundingBoxInWorld.y <= 0.5){

            if (velocity.x != 0){
                // hit the ground running
                setActiveState( PlayerStateEnum::WALKING );
                boundingBoxInWorld.y = 0.5;
                velocity.y = 0;

            } else {

                setActiveState( PlayerStateEnum::IDLE );
                boundingBoxInWorld.y = 0.5;

                velocity.x = 0;
                velocity.y = 0;
            }
            
        }



    } else if ( activeState == PlayerStateEnum::WALKING ){

        // only keep the Walking state if some direction key
        // is actually being pressed -> otherwise problems may 
        // occur when we summon that state woithout player input ->
        //  e.g. : When falling with X Velocity.
        if (directionKeysPressed){

            int dt = SDL_GetTicks() - last_tick;

            boundingBoxInWorld.x += velocity.x * 0.001 * (float)dt;
            boundingBoxInWorld.y += velocity.y * 0.001 * (float)dt;
        
        } else {

            setActiveState(PlayerStateEnum::IDLE);
        
        }

    } else if ( activeState == PlayerStateEnum::IDLE ){
        velocity.x = 0;
        velocity.y = 0;
    }

    // update animation
    SpriteThing::update();
}

void PlayerThing::render(){

    SpriteThing::render();

    if (debugStroke){

        SDL_SetRenderDrawColor(context->renderer, 18,200,100,150);
        SDL_RenderDrawRect( context->renderer, &boundingBoxInCanvas );

        // hit box
        SDL_SetRenderDrawColor(context->renderer, 180,150,130,180);

        SDL_Rect internalBB = {
            .w = boundingBoxInCanvas.w - boxOffset.pxOffsetLeft - boxOffset.pxOffsetRight,
            .h = boundingBoxInCanvas.h - boxOffset.pxOffsetDown - boxOffset.pxOffsetUp,
            .x = boundingBoxInCanvas.x + boxOffset.pxOffsetLeft,
            .y = boundingBoxInCanvas.y + boxOffset.pxOffsetUp
        };
        SDL_RenderDrawRect( context->renderer, &internalBB );

    }


}

void PlayerThing::handleEvent( const SDL_Event& e ){

    // key up
    if (e.type == SDL_KEYDOWN){


        if (e.key.keysym.sym == SDLK_SPACE && ( activeState != PlayerStateEnum::FALLING && activeState != PlayerStateEnum::JUMPING ) ){
            
            // DO JUMP CONTROL
            velocity.y = 5;
            setActiveState(PlayerStateEnum::JUMPING);

        } else if (e.key.keysym.sym == SDLK_RIGHT && ( activeState == PlayerStateEnum::FALLING || activeState == PlayerStateEnum::JUMPING )){

            moveForward = true;
            // AIRTIME X INCREMENTS
            if (velocity.x < 1){

                velocity.x += 0.5;
            
            }

            directionKeysPressed = true;
            
        } else if (e.key.keysym.sym == SDLK_LEFT && ( activeState == PlayerStateEnum::FALLING || activeState == PlayerStateEnum::JUMPING )){

            // face correct way
            moveForward = false;

            // AIRTIME X INCREMENTS
            if (velocity.x > -1){

                velocity.x -= 0.5;
            
            }
            
            directionKeysPressed = true;

        } else if ( e.key.keysym.sym == SDLK_RIGHT ) {
            // WALK!
            moveForward = true;

            setActiveState(PlayerStateEnum::WALKING);
            // if (velocity.x < 2){ velocity.x += 0.5;  }
            velocity.x = 2;
            directionKeysPressed = true;

        } else if ( e.key.keysym.sym == SDLK_LEFT ) {
            // WALK!
            moveForward = false;

            setActiveState(PlayerStateEnum::WALKING);
            velocity.x = -2;
            directionKeysPressed = true;

        }


    } else if (e.type == SDL_KEYUP){
        if (e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_RIGHT){
            
            directionKeysPressed = false;
                
            if (activeState == PlayerStateEnum::WALKING){
                setActiveState(PlayerStateEnum::IDLE);
                velocity.x = 0;
            }
            
        }
    }


}

void PlayerThing::initState(PlayerStateEnum stateIndex, SpriteSheetAnimation animation){

    assert(stateIndex < 5 && stateIndex > 0);
    states[stateIndex] = animation;

}

void PlayerThing::setActiveState(PlayerStateEnum stateIndex){

    assert(stateIndex < 5 && stateIndex >= 0);

    if (stateIndex != activeState){
        activeState = stateIndex;
        setActiveAnimation( stateIndex );
    }
}
