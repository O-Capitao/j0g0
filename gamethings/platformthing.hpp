#pragma once
#include <vector>

#include "graphics/gamegeometry.hpp"
#include "tilemapthing.hpp"

namespace j0g0{

    // A collection of points in the 
    // World Space that will describe the platform's outer
    // surfaces
    struct PlatformSurface{

        int upperBoundPoint_Count, lowerBoundPoint_Count,
            leftBoundPoint_Count, rightBoundPoint_Count;

        Vec2D_Float *upperBound, *lowerBound, *leftBound, *rightBound;
        std::vector<Vec2D_Float> outline;

        PlatformSurface();
        ~PlatformSurface();

        void _init( const std::vector<Vec2D_Float> &_outline );

    };






    struct PlatformThing: public TileMapThing{
        
        PlatformThing(RenderingContext* _context,
            std::string path_to_img,
            int n_frames,
            int x_divisions,
            int y_divisions,
            ViewPort* _viewPort_p);

        ~PlatformThing();

        PlatformSurface surface;

    };
}
