#include <iostream>
#include <math.h>

#include "viewport.hpp"

using namespace j0g0;

ViewPort::ViewPort( const ViewPortProperties &vp_conf, const CanvasProperties& CanvasProperties ){
    
    positionInWorld = vp_conf.worldPosition;
    
    canvasSize = CanvasProperties.size;

    float _canvasAspectRatio = (float)canvasSize.x / (float)canvasSize.y;

    sizeInWorld = {
        .x = vp_conf.width,
        .y = vp_conf.width / _canvasAspectRatio
    };

    pixel_per_meter = (float) canvasSize.x / sizeInWorld.x;

}

/*
 * Transforms
         Vec2D_Float worldToViewPort(const Vec2D_Float& worldPosition);
            - same scale
            - same orientation -> offset only


        Vect2D_Int viewPortToCanvas(const Vec2D_Float& viewportPosition);
            - scaled
            - flipped y

 */ 
float ViewPort::scaleToWorld(const int &lengthInCanvas){
    return ((float)lengthInCanvas) / pixel_per_meter;
}

int ViewPort::scaleToCanvas(const float &lengthInCanvas){
    return (int)floor( lengthInCanvas * pixel_per_meter );
}

Vec2D_Float ViewPort::worldToViewPort(const Vec2D_Float& worldPosition){
    return { 
        .x = worldPosition.x - positionInWorld.x, 
        .y= worldPosition.y - positionInWorld.y 
    };
}

Vec2D_Int ViewPort::viewPortToCanvas(const Vec2D_Float& viewportPosition){
    return {
        .x = (int)floor( viewportPosition.x * pixel_per_meter),
        .y = (int)floor( (sizeInWorld.y - (float)viewportPosition.y) * pixel_per_meter )
    };
}

// cnvs = (size - vpposi ) ppm
// cnvs/ppm = size - vp
// vp = size - cnvs/ppm
Vec2D_Float ViewPort::canvasToViewport( const Vec2D_Int& canvasPosition ){
    return {
        .x = (float)canvasPosition.x / pixel_per_meter,
        .y = sizeInWorld.y - ((float)(canvasPosition.y) / pixel_per_meter)
    };
}

Vec2D_Float ViewPort::viewPortToWorld( const Vec2D_Float& viewportPosition ){
    return {
        .x = viewportPosition.x + positionInWorld.x,
        .y = viewportPosition.y + positionInWorld.y
    };
}

/***
 *  BOX TRANSFORMS
 */
FloatRect ViewPort::transformRect_Canvas2Viewport( const SDL_Rect& boxInCanvas ){
    return {
        .w = (float)boxInCanvas.w / pixel_per_meter,
        .h = (float)boxInCanvas.h / pixel_per_meter,
        .x = (float)boxInCanvas.x / pixel_per_meter,
        .y = (float)( canvasSize.y - boxInCanvas.y - boxInCanvas.h) / pixel_per_meter
    };
}

FloatRect ViewPort::transformRect_ViewPort2World( const FloatRect& boxInViewport ){
    return {
        .w = boxInViewport.w,
        .h = boxInViewport.h,
        .x = boxInViewport.x + this->positionInWorld.x,
        .y = boxInViewport.y + this->positionInWorld.y,
    };
}

FloatRect ViewPort::transformRect_World2Viewport( const FloatRect& boxInWorld ){
    return {
        .w = boxInWorld.w,
        .h = boxInWorld.h,
        .x = boxInWorld.x - positionInWorld.x,
        .y = boxInWorld.y - positionInWorld.y
    };
}

SDL_Rect ViewPort::transformRect_Viewport2Canvas( const FloatRect& boxInViewport ){
    return{
        .w = (int)floor( boxInViewport.w * this->pixel_per_meter ),
        .h = (int)floor( boxInViewport.h * this->pixel_per_meter ),
        .x = (int)floor( boxInViewport.x * this->pixel_per_meter ), 
        .y = (int)floor( canvasSize.y - pixel_per_meter * ( boxInViewport.h + boxInViewport.y ) )
    };
}

FloatRect ViewPort::transformRect_canvasToWorld( const SDL_Rect& boxInCanvas ){
    return transformRect_ViewPort2World(
        transformRect_Canvas2Viewport( boxInCanvas )
    );
}

SDL_Rect ViewPort::transformRect_WorldToCanvas( const FloatRect& boxInWorld ){
    return transformRect_Viewport2Canvas(
        transformRect_World2Viewport( boxInWorld )
    );
}


