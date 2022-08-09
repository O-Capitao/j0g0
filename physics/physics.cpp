#include "physics.hpp"

#include "math.h"
#include "iostream"

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
    _acceleration = {.x = 0, .y=-9.8};
    _terminalVelocity = 50;

    box = b;

    _fallingMode = TrajectoryType::PROJECTILE;

    _currentGroundLine_p = nullptr;
    _currentGroundVelocity_p = nullptr;

    // objects start falling -> on the 1st step we set them in Ground
    FALLING = true;

    isBounceable = _isBounceable;
}

void BoxPhysicsModel::update(float dt){

    if (FALLING){

        if ( abs(_velocity.x /2 + _velocity.y/2) < _terminalVelocity ){
            _velocity.x += _acceleration.x * dt ;
            _velocity.y += _acceleration.y * dt ;
        }
        box.x += dt * _velocity.x;
        box.y += dt *_velocity.y;   
    } else {
        
        box.x += dt * (_velocity.x + _currentGroundVelocity_p->x); 
    }
}

void BoxPhysicsModel::snapToLine(
    StraightFloatLine *l,
    SnappedPointType spType,
    Vec2D_Float *targetPlatformVelocity_p
){

    // metod 1 - find penetration and restore it in movement axis
    float penetration;

    // we can't land on Vertical Line
    if (l->horizontalLength == 0){

        if (_velocity.x > 0){

            penetration = l->point_a.x  - (box.x + box.w);
        } else {
            penetration =  l->point_a.x - box.x;
        }

        // restore penetration 
        box.x += penetration;

        // bounce back?

    // Horizonal Lines.
    } else {
        
        // Hit  the bott of Platform with Top of Box - No Landing
        if (_velocity.y > 0){
        
            penetration = l->point_a.y - (box.y + box.h);

            // restore penetration 
            box.y += penetration;
        
        } else {

            // Case for collision with Hor. Line from above
            // possibility to land/ slide whatev. Change state
            penetration = l->point_a.y - box.y;

            // restore penetration 
            box.y += penetration;

            // the box has landed
            FALLING = false;

            _currentGroundLine_p = l;
            _currentGroundVelocity_p = targetPlatformVelocity_p;
        }
    }
}

bool BoxPhysicsModel::checkIfInBounds(){
    return (box.y + box.h < 0);
}

void BoxPhysicsModel::applyDv( const Vec2D_Float &dv ){
    _velocity.x += dv.x;
    _velocity.y += dv.y;
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
    // updateBorders();
}



/****
 * 
 *  The Simulation
 * 
 ***/
std::optional<ObjectToPlatformCollisionPair> PlatformGamePhysics::_findObstacle( BoxPhysicsModel* obj ){

    if (obj->_velocity.x == 0 && obj->_velocity.y == 0){
        return std::nullopt;
    }

    if (!obj->FALLING){
        return std::nullopt;
    }

    // do it naive fr now
    for ( RectPlatformPhysicsModel *p : platforms ){
        // check for collision in present frame
        if ( (p->box).collidesWith( obj->box ) ){

            // what line did we breach
            StraightFloatLine *candidate;
            float distance_to_surface_x, distance_to_surface_y;

            // check for vert surface
            if (obj->_velocity.x < 0){
                candidate = &(p->instantBorders[3]);
                distance_to_surface_x = obj->box.x - candidate->point_a.x;
            } else {
                candidate = &(p->instantBorders[1]);
                distance_to_surface_x = candidate->point_a.x - (obj->box.x + obj->box.w);
            }


            if (obj->_velocity.y < 0){
                
                distance_to_surface_y = obj->box.y - p->instantBorders[0].point_a.y;

                if ( abs(distance_to_surface_y) < abs(distance_to_surface_x)){
                    candidate = &(p->instantBorders[0]);
                }
                
            } else {

                distance_to_surface_y = p->instantBorders[0].point_a.y - obj->box.y;
                if ( abs(distance_to_surface_y) < abs(distance_to_surface_x) ){
                    candidate = &(p->instantBorders[2]);
                }
            }

            ObjectToPlatformCollisionPair res = {
                .box_p = obj,
                .plat_p = p,
                .platformBoundary = candidate
            };

            return res;

        }
    }

    return std::nullopt;
}

void PlatformGamePhysics::resolveModel( Uint32 dt_ms ){

    // float dt_s = 0.001 * (float)dt_ms;

    for (BoxPhysicsModel* box_p : objects){

        // resolve collisions
        if (auto result = _findObstacle(box_p)) {

            ObjectToPlatformCollisionPair result_value = result.value();

            if (result_value.plat_p->ellasticCoef>0){
                // there can be some bounce back
                // vertical impact

                if (result_value.platformBoundary->verticalLength == 0 && abs( box_p->_velocity.y ) > VEL_THRESHOLD){
                
                    box_p->box.y = result_value.platformBoundary->point_a.y;

                    box_p->_velocity.y =  box_p->isBounceable ? 
                        -result_value.plat_p->ellasticCoef * box_p->_velocity.y
                        : 0;
                
                } else if (result_value.platformBoundary->horizontalLength == 0 && abs( box_p->_velocity.x ) > VEL_THRESHOLD){
                
                    box_p->_velocity.x = box_p->isBounceable ? 
                        -result_value.plat_p->ellasticCoef * box_p->_velocity.x
                        : 0;
                
                } else {

                    box_p->snapToLine(
                        result_value.platformBoundary,
                        SnappedPointType::BOTTOM_CENTER,
                        &(result_value.plat_p->velocity)
                    );
                }

            } else {

                box_p->snapToLine(
                    result_value.platformBoundary,
                    SnappedPointType::BOTTOM_CENTER,
                    &(result_value.plat_p->velocity)
                );

            }
        }

        // resolve ground/fall
        if (!box_p->FALLING){
            StraightFloatLine *boxGround = box_p->_currentGroundLine_p;
            if ( (box_p->box.x > (boxGround->point_a.x + boxGround->horizontalLength)) || ( (box_p->box.x + box_p->box.w) < boxGround->point_a.x  ) ){

                printf("Ran out of ground: box.x: %f - End of Box: %f - Beg Of Plat: %f  - End of Plat: %f\n", 
                (box_p->box.x), (box_p->box.x + box_p->box.w), (boxGround->point_a.x), (boxGround->point_a.x + boxGround->horizontalLength) );

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

