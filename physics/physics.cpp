#include "physics.hpp"

using namespace j0g0;

BoxPhysicsModel::BoxPhysicsModel(
    float m,
    float fc,
    const Vec2D_Float &p,
    const FloatRect &b) {
    
    _mass = m;
    _frictionCoef = fc;
    _velocity = {.x = 0, .y=0};
    _acceleration = {.x = 0, .y=0};

    box = b;

    _fallingMode = TrajectoryType::PROJECTILE;

    // objects start falling -> on the 1st step we set them in Ground
    FALLING = true;
}

void BoxPhysicsModel::update(float dt){
    
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








RectPlatformPhysicsModel::RectPlatformPhysicsModel(
    const FloatRect &bounding_box,
    float ellastic_coef,
    float friction_coef
):box(bounding_box),
frictionCoef(friction_coef),
ellasticCoef(ellastic_coef)
{
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



// void PlatformGamePhysics::free(){
//     for (auto ap : objects){
//         delete ap;
//     }
//     objects.empty();

//     for (auto pp : platforms){
//         delete pp;
//     }
//     platforms.empty();
// }

// a box flies through the level
// will it collide in the coming update?
//  vs
//      did it collide in the last update?
std::optional<StraightFloatLine> PlatformGamePhysics::_findObstacle( BoxPhysicsModel* obj ){

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

                if ( distance_to_surface_y < distance_to_surface_x ){
                    candidate = p->borders[0];
                }
                
            } else {

                distance_to_surface_y = p->borders[0].point_a.y - obj->box.y;
                if ( distance_to_surface_y < distance_to_surface_x ){
                    candidate = p->borders[2];
                }
            }

            obj->snapToLine(
                candidate,
                SnappedPointType::BOTTOM_CENTER
            );

            return candidate;

        }
    }

    return std::nullopt;
}

void PlatformGamePhysics::resolveModel( float dt ){
    
}