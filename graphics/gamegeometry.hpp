#pragma once

/***
 * Utility Geometry objs
 **/ 
namespace j0g0 {

    struct Vec2D_Int {
        int x, y;
    };

    struct Vec2D_Float {
        float x, y;
    }; 

    struct FloatRect {
        float x,y,w,h;
    };

    struct RotMatrix2D {
        
        float matrix[2][2] = {
            {0,0},
            {0,0}
        };

        RotMatrix2D(float angle_in_rad);

        Vec2D_Float rotateVec2D_Float( const Vec2D_Float& original);

    };

    /**
     * Points should be defined CCW for normals to be pointing outwards
     */
    struct FloatLine {

        FloatLine( const Vec2D_Float& _pA, const Vec2D_Float& _pB );

        Vec2D_Float point_a, point_b;
        Vec2D_Float tangent;
        Vec2D_Float normal;

        bool intersectsWith( const FloatLine& other );

    };
    struct Polygon {

        private:
            int _line_count, _point_count;
            FloatLine *_linesArray;
            Vec2D_Float *_pointsArray;
    };


}