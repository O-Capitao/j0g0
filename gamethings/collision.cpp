#include "collision.hpp"

using namespace j0g0;

// CollisionBox::CollisionBox(const Vec2D_Float& _point_a,const Vec2D_Float& _point_b ){
    
//     pointA = { _point_a.x, _point_a.y };
//     pointB = { _point_b.x, _point_b.y };

//     size = {
//         pointB.x - pointA.x,
//         pointB.y - pointA.y
//     };
    
// }

CollisionBox::CollisionBox(
    const Vec2D_Int& _pointA_Cnvs,
    const Vec2D_Int& _pointB_Cnvs,
    const Vec2D_Float& _viewportSize,
    float pixel_to_meter
){

    pointA_Canvas = { _pointA_Cnvs.x, _pointA_Cnvs.y };
    pointB_Canvas = { _pointB_Cnvs.x, _pointB_Cnvs.y };

    size_Canvas = {
        pointB_Canvas.x - pointA_Canvas.x,
        pointB_Canvas.y - pointA_Canvas.y
    };

    size_World = {
        (float)size_Canvas.x / pixel_to_meter,
        (float)size_Canvas.y / pixel_to_meter
    };

    



}

/**
 * 
 * 
 * if (rect1.x < rect2.x + rect2.w &&
        rect1.x + rect1.w > rect2.x &&
        rect1.y < rect2.y + rect2.h &&
        rect1.h + rect1.y > rect2.y) {
        // collision detected!
 */
bool CollisionBox::collideWith(const CollisionBox& other ){

    // bool collisionDetected = false;

    // if (pointA.x < other.pointB.x 
    //     && pointB.x > other.pointA.x
    //     && pointA.y < other.pointB.y)


    // return collisionDetected;
    return false;
}
