#include "physics.hpp"

#include "math.h"

#include "game-defines.hpp"

using namespace j0g0;

BoxPhysicsModel::BoxPhysicsModel(
    float m,
    float fc,
    const Vec2D_Float &p,
    const FloatRect &b,
    const std::string& ownerId,
    const Vec2D_Float &v0,
    bool _isBounceable )
{
    _ownerId = ownerId;
    _mass = m;
    _frictionCoef = fc;
    _velocity = v0;
    
    _platRelativeVelocity = {.x=0, .y=0};

    _acceleration = {.x = 0, .y = -9.8};
    _terminalVelocity = 50;

    box = b;

    _currentGroundLine_p = nullptr;
    _currentGroundVelocity_p = nullptr;
    _currentGroundOwnerId = "";

    // objects start falling -> on the 1st step we set them in Ground
    FALLING = true;

    isBounceable = _isBounceable;

    _impactAbsortionCoef = 0.5;

    _PENETRATION_TOLERANCE = box.w / 2;
}

void BoxPhysicsModel::update(float dt){


    if (FALLING){

        if ( abs(_velocity.x /2 + _velocity.y/2) < _terminalVelocity ){
            _velocity.x += _acceleration.x * dt ;
            _velocity.y += _acceleration.y * dt ;
        }

    } else {
        
        _velocity.x = _currentGroundVelocity_p->x + _platRelativeVelocity.x;
        _velocity.y = _currentGroundVelocity_p->y;
    
    
    }

    box.x += dt * _velocity.x;
    box.y += dt * _velocity.y; 

}

void BoxPhysicsModel::releaseGround(){
    _currentGroundLine_p = NULL;
    _currentGroundVelocity_p = NULL;
    _currentGroundOwnerId = "";
    FALLING = true;
}

 const Vec2D_Float& BoxPhysicsModel::getVelocity(){
    return _velocity;
}

const std::string &BoxPhysicsModel::getOwnerId(){
    return _ownerId;
}

void BoxPhysicsModel::snapToLine(
    StraightFloatLine *l,
    Vec2D_Float *targetPlatformVelocity_p,
    const std::string &platId
){

    // metod 1 - find penetration and restore it in movement axis
    float penetration;

    // we can't land on Vertical Line
    // include also the case of trying to snap to a line which is over the box's feet
    //      in this case treat like vertical wall
    if (l->horizontalLength == 0 
        || ( l->verticalLength == 0 && _velocity.y < 0 && l->point_a.y > (box.y + _PENETRATION_TOLERANCE) )
    )
    {

        if ( (_velocity.x - targetPlatformVelocity_p->x) > 0){

            penetration = l->point_a.x  - (box.x + box.w);
        } else {
            penetration =  l->point_a.x - box.x;
        }

        // restore penetration 
        box.x += penetration;
        _velocity.x = targetPlatformVelocity_p->x;

        // bounce back?

    // Horizonal Lines.
    } else {
        
        // Hit  the bott of Platform with Top of Box - No Landing
        if (_velocity.y > 0){
        
            penetration = l->point_a.y - (box.y + box.h);

            // // restore penetration 
            box.y += 2 * penetration;
            _velocity.y = (1 - _impactAbsortionCoef) * -_velocity.y;

            // box.y = l->point_a.y; - box.h;
        
        } else {

            // // Case for collision with Hor. Line from above
            // // // possibility to land/ slide whatev. Change state
            penetration =  l->point_a.y - box.y;

            // // restore penetration 
            box.y += penetration;

            #if DEBUG
            printf("Snapping to Line: pA.x=%f , pa.y=%f \n", l->point_a.x, l->point_a.y );
            #endif

            // the box has landed
            FALLING = false;

            _velocity = {
                .x = targetPlatformVelocity_p->x, .y = targetPlatformVelocity_p->y
            };

            _currentGroundLine_p = l ;
            _currentGroundVelocity_p = targetPlatformVelocity_p;
            _currentGroundOwnerId = platId;

         }
    }
}

bool BoxPhysicsModel::checkIfInBounds(){
    return (box.y + box.h < 0);
}

void BoxPhysicsModel::jump( const Vec2D_Float &dv ){
        assert(!FALLING);
        
        _velocity.x += ( _currentGroundVelocity_p->x + dv.x );
        _velocity.y += ( _currentGroundVelocity_p->y +  dv.y );

        FALLING = true;
        releaseGround();

        // give this to get some clear space between
        // the jumping Player and the potentially moving platform under him
        // and avoid firing the collision detector.
        box.y+=0.1;
    }


void BoxPhysicsModel::setWalkingVelocity(float walkingVelocity){
    if (!FALLING){
        _platRelativeVelocity.x = walkingVelocity;
    }
}

RectPlatformPhysicsModel::RectPlatformPhysicsModel(
    const FloatRect &bounding_box,
    float ellastic_coef,
    float friction_coef,
    const std::string &ownerId
):box(bounding_box),
frictionCoef(friction_coef),
ellasticCoef(ellastic_coef)
{   
    _ownerId = ownerId;

    initBorders();

    velocity = {
        .x = 0, .y = 0
    };

    acceleration = {
        .x = 0, .y = 0
    };
}

void RectPlatformPhysicsModel::initBorders(){
       // UP
    // fill matrix of border lines
    bordersShiftedToOrigin[0] = {
        .point_a = { .x = 0, .y = box.h },
        .normal = { .x = 0, .y = 1 },
        .horizontalLength = box.w,
        .verticalLength = 0
    };
    // LEFT
    // fill matrix of border lines
    bordersShiftedToOrigin[1] = {
        .point_a = { .x = 0, .y = 0 },
        .normal = { .x = -1, .y = 0 },
        .horizontalLength = 0,
        .verticalLength = box.h
    };
    // DWN
    // fill matrix of border lines
    bordersShiftedToOrigin[2] = { 
        .point_a = { .x = 0, .y = 0 },
        .normal = { .x = 0, .y = -1 },
        .horizontalLength = box.w,
        .verticalLength = 0
    };
    // RIGHT
    // fill matrix of border lines
    bordersShiftedToOrigin[3] = {
        .point_a = { .x = box.w, .y = 0 },
        .normal = { .x = 1, .y = 0 },
        .horizontalLength = 0,
        .verticalLength = box.h
    };

    for (int i = 0 ; i < 4; i++){
        instantBorders[i] = bordersShiftedToOrigin[i];
    }

    updateBorders();

}

// keep the data structures in the same memory space -> otherwise
// Actors standing in this plat will have stale pointers
void RectPlatformPhysicsModel::updateBorders(){
    for (int i = 0 ; i < 4; i++){
        instantBorders[i].point_a.x = box.x + bordersShiftedToOrigin[i].point_a.x;
        instantBorders[i].point_a.y = box.y + bordersShiftedToOrigin[i].point_a.y;
    }
}

void RectPlatformPhysicsModel::setVelocity(const Vec2D_Float &_newVelocity){
    velocity.x = _newVelocity.x;
    velocity.y = _newVelocity.y;
}

void RectPlatformPhysicsModel::update(float dt_s){
    if (velocity.x != 0 || velocity.y != 0){

        box.x += velocity.x * dt_s;
        box.y += velocity.y * dt_s;

        updateBorders();
    }
}



// More proper evaluation of a overlap state between 1 box and n platforms/rectangular objects
// moving at some (or no) velocity.
std::vector<ObjectToPlatformCollisionPair> PlatformGamePhysics::_findObstacles( BoxPhysicsModel* obj ){

    std::vector<ObjectToPlatformCollisionPair> retval;

    // do it naive fr now
    for ( int i = 0; i < platforms.size(); i++ ){

        auto p = platforms[i];

        if (!obj->FALLING && obj->_currentGroundOwnerId == p->getOwnerId() ){
            // DO nothing in the case
            // that the box is not falling
            // and the collision that was detected is the ground
            // the box stands on.
        
        }else if (
            (p->box).overlapsWith( obj->box )    
        ){
            ObjectToPlatformCollisionPair res = {
                .box_p = obj,
                .plat_p = p,
                .platformBoundary = _getHitFace( obj, p )
            };

            retval.push_back( res );
            
        }
    }

    return retval;
}



void PlatformGamePhysics::resolveModel( Uint32 dt_ms ){

    for (BoxPhysicsModel* box_p : objects){

        std::vector<ObjectToPlatformCollisionPair> obstacles = _findObstacles(box_p);

        if (obstacles.size()){

            for (ObjectToPlatformCollisionPair collisionPair : obstacles){

                bool nonNegligibleYVelocity = abs( box_p->_velocity.y - collisionPair.plat_p->velocity.y ) > VEL_THRESHOLD;
                bool nonNegligibleXVelocity = abs( box_p->_velocity.x - collisionPair.plat_p->velocity.x ) > VEL_THRESHOLD;

                if (box_p->FALLING){

                    if (collisionPair.plat_p->ellasticCoef>0 && box_p->isBounceable && (nonNegligibleYVelocity || nonNegligibleXVelocity)){
                        // there can be some bounce back
                        // vertical impact

                        if (collisionPair.platformBoundary->verticalLength == 0 && nonNegligibleYVelocity){
                        
                            box_p->box.y = collisionPair.platformBoundary->point_a.y;
                            box_p->_velocity.y =  -collisionPair.plat_p->ellasticCoef * box_p->_velocity.y;
                        
                        } else if (collisionPair.platformBoundary->horizontalLength == 0 && nonNegligibleXVelocity){
                            
                            box_p->box.x = collisionPair.platformBoundary->point_a.x - box_p->box.w;
                            box_p->_velocity.x = -collisionPair.plat_p->ellasticCoef * box_p->_velocity.x;
                        
                        }

                    } else {

                        box_p->snapToLine(
                            collisionPair.platformBoundary,
                            &(collisionPair.plat_p->velocity),
                            collisionPair.plat_p->getOwnerId()
                        );

                    }

                } else {
                    // box is moving along a surface already.
                    // we will not snap, only stop the movement
                    if (collisionPair.platformBoundary->horizontalLength == 0){

                        box_p->_velocity.x = 0;
                        box_p->box.x = collisionPair.platformBoundary->normal.x > 0 ?
                            (collisionPair.platformBoundary->point_a.x)
                            : (collisionPair.platformBoundary->point_a.x - box_p->box.w);

                    }
                }
            }
        }

        // resolve ground/fall
        if (!box_p->FALLING){

            StraightFloatLine *boxGround = box_p->_currentGroundLine_p;
            if ( (box_p->box.x > (boxGround->point_a.x + boxGround->horizontalLength)) || ( (box_p->box.x + box_p->box.w) < boxGround->point_a.x  ) ){
#if DEBUG
                printf("Ran out of ground: box.x: %f - End of Box: %f - Beg Of Plat: %f  - End of Plat: %f\n", 
                (box_p->box.x), (box_p->box.x + box_p->box.w), (boxGround->point_a.x), (boxGround->point_a.x + boxGround->horizontalLength) );
#endif

                box_p->FALLING = true;
                box_p->releaseGround();

            }
        }
    }
}

void PlatformGamePhysics::removeObject( const std::string id ){
    for ( auto it = objects.begin(); it != objects.end(); ){
        if ((*it)->_ownerId == id){
        //    delete * it;
           it = objects.erase(it); 
        } else{
            ++it;
        }
    }
}

const float TOLERANCE = 1e-3;

 StraightFloatLine* PlatformGamePhysics::_getHitFace( BoxPhysicsModel* b, RectPlatformPhysicsModel* p ){

    int borderIndex = 4; // a bad result...

    Vec2D_Float relVel  = {
        .x = b->_velocity.x - p->velocity.x,
        .y = b->_velocity.y - p->velocity.y
    };

    if (abs(relVel.x) < TOLERANCE){
        borderIndex = relVel.y > 0  ? 2 : 0;
    } else if (abs(relVel.y) < TOLERANCE){
        borderIndex = relVel.x > 0 ? 1 : 3;
    } else {

        // oblique collision via dot product
        // init counters for index 0 = TOP.
        // SPECIAL CASE . we will only detect a TOP hit
        // if the box's feet are located over the TOP of the ºplatform (plus tolerance
        float winnerProjection = 0;
        if ( b->_velocity.y < 0 && p->instantBorders[0].point_a.y < (b->box.y + b->_PENETRATION_TOLERANCE )){
            winnerProjection = relVel.calcScalarProductWith( p->instantBorders[0].normal );
        }
        
        int winnerIndex = 0;
        float candidate = 0;
        
        // not clear which one..
        for (int i = 1; i < 4; i++){
            
            candidate = relVel.calcScalarProductWith( p->instantBorders[i].normal );
            if (candidate < winnerProjection){
                winnerProjection = candidate;
                winnerIndex = i;
            }
        }

        borderIndex =  winnerIndex;
    }

    assert(borderIndex != 4);

    return &(p->instantBorders[borderIndex]);
 }
