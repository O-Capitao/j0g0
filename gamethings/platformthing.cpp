#include "platformthing.hpp"

using namespace j0g0;

PlatformSurface::PlatformSurface(){

}

PlatformSurface::~PlatformSurface(){
    delete[] upperBound;
    delete[] lowerBound;
    delete[] leftBound;
    delete[] rightBound;
}

void PlatformSurface::_init(const std::vector<Vec2D_Float> &_outline){
    
}

PlatformThing::PlatformThing(
    RenderingContext* _context,
    std::string path_to_img,
    int n_frames,
    int x_divisions,
    int y_divisions,
    ViewPort* _viewPort_p 
):TileMapThing( _context, path_to_img, n_frames, x_divisions, y_divisions, _viewPort_p ){


    
}
