#include <math.h>
#include "gamegeometry.hpp"

# define PI           3.14159265358979323846  /* pi */

using namespace j0g0;

FloatLine::FloatLine( const Vec2D_Float& _pA, const Vec2D_Float& _pB ) {

    this->point_a = {
        .x = _pA.x,
        .y = _pA.y
    };

    this->point_b = {
        .x = _pB.x,
        .y = _pB.y
    };

    tangent = {
        point_b.x - point_a.x,
        point_b.y - point_a.y
    };

    RotMatrix2D rotMat( - 0.5 * PI );

    normal = rotMat.rotateVec2D_Float( tangent );
    
}


bool FloatLine::intersectsWith( const FloatLine& other ){

}


RotMatrix2D::RotMatrix2D(float angle_in_rad){
    matrix[0][0] = cos( angle_in_rad );
    matrix[0][1] = - sin ( angle_in_rad );
    matrix[1][0] = sin( angle_in_rad );
    matrix[1][1] = cos( angle_in_rad );

}

Vec2D_Float RotMatrix2D::rotateVec2D_Float( const Vec2D_Float& original ){
    return {
        .x = matrix[0][0] * original.x + matrix[0][1] * original.y,
        .y= matrix[1][0] * original.x + matrix[1][1] * original.y
    };
}