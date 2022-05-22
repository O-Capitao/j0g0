#pragma once

#include "graphics/gamegeometry.hpp"

namespace j0g0{
    struct CollisionBox {

        CollisionBox(){}

        CollisionBox(
            const Vec2D_Int& _pointA_Cnvs,
            const Vec2D_Int& _pointB_Cnvs,
            const Vec2D_Float& _viewportSize,
            float pixel_to_meter
        );
        // CollisionBox(const Vec2D_Float& _point_a,const Vec2D_Float& _point_b );
        // Canvas
        /*
         *         -------------- PB
         *         |             |
         *         |             |
         *     PA  --------------- 
         */
        Vec2D_Float pointA_World, pointB_World;
        Vec2D_Int pointA_Canvas, pointB_Canvas;

        Vec2D_Float size_World;
        Vec2D_Int size_Canvas;

        bool blockedLeft = false;
        bool blockedRight = false;
        bool blockedUp = false;
        bool blockedDown = false;

        bool collideWith( const CollisionBox& other );

    };



    struct BoxOffset {
        // in pixel, relative to outer Thing BB
        int pxOffsetUp, 
        pxOffsetDown, 
        pxOffsetLeft, 
        pxOffsetRight;
    };
}