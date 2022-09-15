#include <math.h>

#include "game-defines.hpp"
#include "gamegeometry.hpp"



/* The SAME_SIGNS macro assumes arithmetic where the exclusive-or    */
/* operation will work on sign bits.  This works for twos-complement,*/
/* and most other machine arithmetic.                                */
#define SAME_SIGNS( a, b ) \
	(((long) ((unsigned long) a ^ (unsigned long) b)) >= 0 )



using namespace j0g0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
FloatLine::FloatLine( const Vec2D_Float& _pA, const Vec2D_Float& _pB, bool definedCounterClockwise ) {

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

    RotMatrix2D rotMat((definedCounterClockwise ? -1: 1) * 0.5 * PI );

    normal = rotMat.rotateVec2D_Float( tangent );
    
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool FloatLine::intersectsWith( const FloatLine& other, Vec2D_Float &intersection ){
    
    // ADAPTED FROM C CODE:
    /* Faster Line Segment Intersection   */
    /* Franklin Antonio                   */
    float Ax = point_b.x - point_a.x;
    float Bx = other.point_a.x - other.point_b.x;

    float x1lo,x1hi,y1lo,y1hi;

    if(Ax<0) {						/* X bound box test*/
        x1lo= point_b.x; 
        x1hi= point_a.x;
    } else {
        x1hi= point_b.x; 
        x1lo= point_a.x;
    }
        
    if(Bx>0) {

        if (x1hi < other.point_b.x || other.point_a.x < x1lo) return false;
        
    } else {

        if(x1hi < other.point_a.x || other.point_b.x < x1lo) return false;
    
    }

    float Ay = point_b.y - point_a.y;
    float By = other.point_a.y - other.point_b.y;

    if(Ay<0) {						/* Y bound box test*/
        y1lo=point_b.y; 
        y1hi=point_a.y;
    } else {
        y1hi=point_b.y; 
        y1lo=point_a.y;
    }
    
    if(By>0) {
        if(y1hi < other.point_b.y || other.point_a.y < y1lo) return false;
    } else {
        if(y1hi < other.point_a.y || other.point_b.y < y1lo) return false;
    }

    float Cx = point_a.x - other.point_a.x;
    float Cy = point_a.y - other.point_a.y;

    float f = Ay * By - Ax * By;

    if ( f == 0) return false; // PARALELL LINES

    float d = By*Cx - Bx*Cy;					/* alpha numerator*/
    if(f>0) {						/* alpha tests*/
        if(d<0 || d>f) return false;
    } else {
        if(d>0 || d<f) return false;
    }

    float e = Ax*Cy - Ay*Cx;					/* beta numerator*/
    if(f>0) {						/* beta tests*/
        if(e<0 || e>f) return false;
    } else {
        if(e>0 || e<f) return false;
    }

    /*compute intersection coordinates*/

    float num = d*Ax;						/* numerator */
    float offset = SAME_SIGNS(num,f) ? f/2 : -f/2;		/* round direction*/
    intersection.x = point_a.x + (num+offset) / f;				/* intersection x */

    num = d*Ay;
    offset = SAME_SIGNS(num,f) ? f/2 : -f/2;
    intersection.y = point_a.y + (num+offset) / f;				/* intersection y */

    return true;

}




///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
RotMatrix2D::RotMatrix2D(float angle_in_rad){
    matrix[0][0] = cos( angle_in_rad );
    matrix[0][1] = - sin ( angle_in_rad );
    matrix[1][0] = sin( angle_in_rad );
    matrix[1][1] = cos( angle_in_rad );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
Vec2D_Float RotMatrix2D::rotateVec2D_Float( const Vec2D_Float& original ){
    return {
        .x = matrix[0][0] * original.x + matrix[0][1] * original.y,
        .y= matrix[1][0] * original.x + matrix[1][1] * original.y
    };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
Polygon::Polygon( std::vector<Vec2D_Float> outline, bool definedCounterClockwise){

    _vertexCount = outline.size();
    
    int _local_counter = 0;

    for (const Vec2D_Float& point : outline){
        
        Vec2D_Float pointClone = {
            .x = point.x,
            .y = point.y
        };

        points.push_back(pointClone);

        if (_local_counter < _vertexCount - 1){

            const Vec2D_Float& next = outline[_local_counter+1];
            FloatLine line( pointClone, next, definedCounterClockwise );

            lines.push_back(line);

            _local_counter++;
        }

        
    }

    // Final line connects the last point
    // to the 1st
    FloatLine finalLine( points.back(), points.front(), definedCounterClockwise  );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<FloatLine*> Polygon::getLinesFacingAgainst( Vec2D_Float direction ){

    std::vector<FloatLine*> retval;

    for (FloatLine _l : lines){
        if ( _l.normal.x * direction.x < 0 || _l.normal.y * direction.y < 0 ){
            retval.push_back( &_l );
        }
    }

    return retval;

}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void Vec2D_Float::setBothCoordsTo(const float &val){
    x = val;
    y = val;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
Vec2D_Float Vec2D_Float::calcNormalized(){
    float norm = calcLength();
    return {
        .x = this->x / norm,
        .y = this->y / norm
    };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
float Vec2D_Float::calcLength(){
    return pow(this->x,2) + pow(this->y,2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
const float TOLERANCE = 0.01;
bool Vec2D_Float::isParalelTo(const Vec2D_Float &o){

    return ((y/x) - (o.y/o.x)) > TOLERANCE;
    
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool Vec2D_Float::equals(const Vec2D_Float &o){
    return x ==o.x && y == o.y;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
float Vec2D_Float::calcScalarProductWith(const Vec2D_Float &o){
    return x * o.x + y * o.y;
}