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
    const Vec2D_Float &v0 )
{
    _ownerId = ownerId;
    _mass = m;
    _frictionCoef = fc;
    _velocity = v0;
    _acceleration = {.x = 0, .y=-9.8};
    _terminalVelocity = 50;

    box = b;

    _fallingMode = TrajectoryType::PROJECTILE;

    // objects start falling -> on the 1st step we set them in Ground
    FALLING = true;
}

void BoxPhysicsModel::update(float dt){

    if (FALLING){

        if ( abs(_velocity.x /2 + _velocity.y/2) < _terminalVelocity ){
            _velocity.x += _acceleration.x * dt ;
            _velocity.y += _acceleration.y * dt ;
        }
        box.x += dt * _velocity.x;
        box.y += dt *_velocity.y;   
    }
}

void BoxPhysicsModel::snapToLine(
    const StraightFloatLine &l,
    SnappedPointType spType
){

    // metod 1 - find penetration and restore it in movement axis
    float penetration;

    if (l.horizontalLength == 0){
        if (_velocity.x > 0){

            penetration = l.point_a.x  - (box.x + box.w);
        } else {
            penetration =  l.point_a.x - box.x;
        }

        // restore penetration 
        box.x += penetration;

        // bounce back?


    } else {
        
        if (_velocity.y > 0){
        
            penetration = l.point_a.y - (box.y + box.h);

            // restore penetration 
            box.y += penetration;
        
        } else {
            // Case for collision with Hor. Line from above
            // possibility to land/ slide whatev. Change state
            penetration = l.point_a.y - box.y;

            // restore penetration 
            box.y += penetration;

            // the box has landed
            FALLING = false;

            if (_currentGround != nullptr){
                delete _currentGround;
            }
            _currentGround = new StraightFloatLine(l);

        }
    }
}

bool BoxPhysicsModel::checkIfInBounds(){
    return (box.y + box.h < 0);
}






RectPlatformPhysicsModel::RectPlatformPhysicsModel(
    const FloatRect &bounding_box,
    float ellastic_coef,
    float friction_coef
):box(bounding_box),
frictionCoef(friction_coef),
ellasticCoef(ellastic_coef)
{
    recalculateBorders();

    velocity = {
        .x = 0, .y = 0
    };

    acceleration = {
        .x = 0, .y = 0
    };
}

void RectPlatformPhysicsModel::recalculateBorders(){
       // UP
    // fill matrix of border lines
    borders[0] = {
        .point_a = {
            .x = box.x, .y = (box.y + box.h)
        },
        .normal = {
            .x = 0,
            .y = 1
        },
        .horizontalLength = box.w,
        .verticalLength = 0
    };
    // LEFT
    // fill matrix of border lines
    borders[1] = {
        .point_a = {
            .x = box.x, .y = box.y
        },
        .normal = {
            .x = -1,
            .y = 0
        },
        .horizontalLength = 0,
        .verticalLength = box.h
    };
    // DWN
    // fill matrix of border lines
    borders[2] = {
        .point_a = {
            .x = box.x, .y = box.y
        },
        .normal = {
            .x = 0,
            .y = -1
        },
        .horizontalLength = box.w,
        .verticalLength = 0
    };
    // RIGHT
    // fill matrix of border lines
    borders[3] = {
        .point_a = {
            .x = (box.x + box.w), .y = box.y
        },
        .normal = {
            .x = 1,
            .y = 0
        },
        .horizontalLength = 0,
        .verticalLength = box.h
    };

}



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
            StraightFloatLine candidate;
            float distance_to_surface_x, distance_to_surface_y;

            // check for vert surface
            if (obj->_velocity.x < 0){
                candidate = p->borders[3];
                distance_to_surface_x = obj->box.x - candidate.point_a.x;
            } else {
                candidate = p->borders[1];
                distance_to_surface_x = candidate.point_a.x - (obj->box.x + obj->box.w);
            }


            if (obj->_velocity.y < 0){
                
                distance_to_surface_y = obj->box.y - p->borders[0].point_a.y;

                if ( abs(distance_to_surface_y) < abs(distance_to_surface_x)){
                    candidate = p->borders[0];
                }
                
            } else {

                distance_to_surface_y = p->borders[0].point_a.y - obj->box.y;
                if ( abs(distance_to_surface_y) < abs(distance_to_surface_x) ){
                    candidate = p->borders[2];
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

        if (auto result = _findObstacle(box_p)) {

            ObjectToPlatformCollisionPair result_value = result.value();

            if (result_value.plat_p->ellasticCoef>0){
                // there can be some bounce back
                // vertical impact

                if (result_value.platformBoundary.verticalLength == 0 && abs( box_p->_velocity.y ) > VEL_THRESHOLD){
                
                    box_p->box.y = result_value.platformBoundary.point_a.y;
                    box_p->_velocity.y = -result_value.plat_p->ellasticCoef * box_p->_velocity.y;
                
                } else if (result_value.platformBoundary.horizontalLength == 0 && abs( box_p->_velocity.x ) > VEL_THRESHOLD){
                
                    box_p->_velocity.x = -result_value.plat_p->ellasticCoef * box_p->_velocity.x;
                
                } else {

                    box_p->snapToLine(
                        result_value.platformBoundary,
                        SnappedPointType::BOTTOM_CENTER
                    );
                }

            } else {

                box_p->snapToLine(
                    result_value.platformBoundary,
                    SnappedPointType::BOTTOM_CENTER
                );

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

