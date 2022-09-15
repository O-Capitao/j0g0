#pragma once

#include <vector>

/***
 * Utility Geometry objs
 **/ 
namespace j0g0 {

    struct Vec2D_Int {
        int x, y;
        const Vec2D_Int &sumWith( const Vec2D_Int &other );
    };

    struct Vec2D_Float {
        
        float x, y;

        Vec2D_Float sumWith( const Vec2D_Float &other );
        Vec2D_Float calcNormalized();
        
        float calcLength();
        float calcScalarProductWith(const Vec2D_Float &other);
        bool isParalelTo(const Vec2D_Float &other);
        bool equals(const Vec2D_Float &other);
        void setBothCoordsTo(const float &val);
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
        
        FloatLine( const Vec2D_Float& _pA, const Vec2D_Float& _pB, bool definedCounterClockwise);

        Vec2D_Float point_a, point_b;
        Vec2D_Float tangent;
        Vec2D_Float normal;

        bool intersectsWith( const FloatLine& other, Vec2D_Float &intersection );

    };
    struct Polygon {

        // Polygon(){};
        Polygon(std::vector<Vec2D_Float> outline, bool definedCounterClockwise);
        
        std::vector<FloatLine*> getLinesFacingAgainst( Vec2D_Float direction );
        
        private:
        
            int _vertexCount;
            std::vector<FloatLine> lines;
            std::vector <Vec2D_Float> points;
    };

    // struct Vertical_FloatLine {
    //     // the point with lowest y coord in line
    //     Vec2D_Float point_a;
    //     float verticalLength;
    // };

    // struct Horizontal_FloatLine {
    //     // the point with lowest x coord in line
    //     Vec2D_Float point_a;
    //     float horizontalLength;
    // };
        
    struct StraightFloatLine {
        // the point with lowest x/y coord in line
        Vec2D_Float point_a;
        Vec2D_Float normal;
        float verticalLength;
        float horizontalLength;
    };
    
    // // TODO - use this?
    // struct CollisionBox {

    //     CollisionBox(){}

    //     CollisionBox(
    //         const Vec2D_Int& _pointA_Cnvs,
    //         const Vec2D_Int& _pointB_Cnvs,
    //         const Vec2D_Float& _viewportSize,
    //         float pixel_to_meter
    //     );
    //     // CollisionBox(const Vec2D_Float& _point_a,const Vec2D_Float& _point_b );
    //     // Canvas
    //     /*
    //      *         -------------- PB
    //      *         |             |
    //      *         |             |
    //      *     PA  --------------- 
    //      */
    //     Vec2D_Float pointA_World, pointB_World;
    //     Vec2D_Int pointA_Canvas, pointB_Canvas;

    //     Vec2D_Float size_World;
    //     Vec2D_Int size_Canvas;

    //     bool blockedLeft = false;
    //     bool blockedRight = false;
    //     bool blockedUp = false;
    //     bool blockedDown = false;

    //     bool collideWith( const CollisionBox& other );

    // };


}