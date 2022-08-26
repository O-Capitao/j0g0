#pragma once

#include "gamegeometry.hpp"
#include "rendering.hpp"
#include "gameconfig.hpp"

namespace j0g0 {

    struct ViewPort {
        
        ViewPort( const ViewPortConfig &vp_conf, const GameWindowConfig& windowConfig );

        Vec2D_Float positionInWorld;
        Vec2D_Float sizeInWorld;
        Vec2D_Int canvasSize;

        // scale transformation
        float scaleToWorld(const int &lengthInCanvas);
        int scaleToCanvas(const float &lengthInCanvas);

        // Point Tranformations
        Vec2D_Float worldToViewPort( const Vec2D_Float& worldPosition );
        Vec2D_Int viewPortToCanvas( const Vec2D_Float& viewportPosition );

        Vec2D_Float canvasToViewport( const Vec2D_Int& canvasPosition );
        Vec2D_Float viewPortToWorld( const Vec2D_Float& viewportPosition );

        // Box Transformations
        FloatRect transformRect_Canvas2Viewport( const SDL_Rect& boxInCanvas );
        FloatRect transformRect_ViewPort2World( const FloatRect& boxInViewport );

        FloatRect transformRect_World2Viewport( const FloatRect& boxInWorld );
        SDL_Rect transformRect_Viewport2Canvas( const FloatRect& boxInViewport);

        FloatRect transformRect_canvasToWorld( const SDL_Rect& boxInCanvas );
        SDL_Rect transformRect_WorldToCanvas( const FloatRect& boxInWorld );

        // private:
        float pixel_per_meter;

    };
}
