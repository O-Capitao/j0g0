#include "platform.hpp"

using namespace j0g0;

Platform::Platform( 
    RenderingContext* _context, 
    SpriteSheet* _spriteSheet, 
    const Vec2D_Int& blockSizeInTiles,
    std::vector<SpriteSlice> tileSet,
    const Vec2D_Float& positionInWorld,
    ViewPort* vp
)
:GameObj(_context),
_spriteSheet_p(_spriteSheet),
_tileMap( _spriteSheet->getSliceSize(), blockSizeInTiles, tileSet ),
_hardOutline( _createOutline(blockSizeInTiles, _spriteSheet->getSliceSize(), vp) ),
_positionInWorld(positionInWorld)
{}

Platform::~Platform(){
    _spriteSheet_p = NULL;
    _viewport_p = NULL;
}

void Platform::render(){

    Vec2D_Int positionCanvas = _viewport_p->viewPortToCanvas( 
        _viewport_p->worldToViewPort( _positionInWorld ) 
    );

    const Vec2D_Int& platformSize = _tileMap.getSizeInTiles();

    for (int _matrix_row = 0; _matrix_row < platformSize.y; _matrix_row++){

        for (int _matrix_col = 0; _matrix_col < platformSize.x; _matrix_col++){

            SpriteSlice slice = _tileMap.getTileAt(_matrix_row, _matrix_col);

            _spriteSheet_p->renderSlice({
                    .x = positionCanvas.x + _matrix_col * slice.frame.w,
                    .y = positionCanvas.y + _matrix_row * slice.frame.h
                }, 
                slice
            );
        }
    }
}

void Platform::setViewport( ViewPort *vp ){
    _viewport_p = vp;
}

Polygon Platform::_createOutline( const Vec2D_Int& blockSizeInTiles, const Vec2D_Int& tileSize, ViewPort *vp ){
    return Polygon({
        {
            .x = 0.0,
            .y = 0.0
        },
        {
            .x = (float)(blockSizeInTiles.x * tileSize.x) / vp->pixel_per_meter,
            .y = 0
        },
        {
            .x = (float)(blockSizeInTiles.x * tileSize.x)/ vp->pixel_per_meter,
            .y = (float)(blockSizeInTiles.y * tileSize.y)/ vp->pixel_per_meter,
        },
        {
            .x = 0,
            .y = (float)(blockSizeInTiles.y * tileSize.y) / vp->pixel_per_meter,
        }
    }, true);
}

