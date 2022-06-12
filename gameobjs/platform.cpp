#include "platform.hpp"

using namespace j0g0;

Platform::Platform( 
    RenderingContext* _cntxt, 
    SpriteSheet* _ss, 
    ViewPort* vp,
    PlatformProperties &p
)
:GameObj(_cntxt),
_properties(p),
_spriteSheet_p(_ss),
_tileMap( _ss->getSliceSize(), p.sizeInTiles, _calculateTileSetFromConfig(), p.tileMapSpriteSliceMatrix ),
_hardOutline( _createOutline(p.sizeInTiles, _ss->getSliceSize(), vp) ),
_viewport_p(vp)
{}

Platform::~Platform(){
    _spriteSheet_p = NULL;
    _viewport_p = NULL;
}

void Platform::render(){

    Vec2D_Int positionCanvas = _viewport_p->viewPortToCanvas( 
        _viewport_p->worldToViewPort( _properties.positionInWorld ) 
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

std::vector<SpriteSlice> Platform::_calculateTileSetFromConfig(){
    
    assert( 
        _properties.tileMapSpriteSliceMatrix.size() == _properties.sliceRotations_in90Deg.size() 
        && _properties.sliceRotations_in90Deg.size() == _properties.sliceFlip_H.size()
        && _properties.sliceFlip_H.size() == (_properties.sizeInTiles.x * _properties.sizeInTiles.y)
    );

    std::vector<SpriteSlice> slices;

    for (int i = 0; i < (_properties.sizeInTiles.x * _properties.sizeInTiles.y); i++){

        SDL_RendererFlip i_flip = _properties.sliceFlip_V[i] ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;

        if (_properties.sliceFlip_H[i]){
            i_flip = SDL_FLIP_HORIZONTAL;
        }

        // TODO - rotation stuff
        SpriteSlice i_slice = {
            .duration_ms = 0,
            .quarter_turns_ccw = (short)_properties.sliceRotations_in90Deg[i],
            .flip = i_flip,
            .offset_for_rotation = {
                .x = 0, .y = 0
            },
            .center_of_rotation = {
                .x = 0, .y = 0
            },
            .frame = _spriteSheet_p->getFrameAt(_properties.tileMapSpriteSliceMatrix[i]).frame
        };
        
        slices.push_back(i_slice);
    }

    return slices;
}

