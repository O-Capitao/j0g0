#include "platform.hpp"

using namespace j0g0;

RectPlatform::RectPlatform( 
    RenderingContext* _cntxt, 
    SpriteSheet* _ss, 
    ViewPort* vp,
    PlatformProperties &p
)
:GameObj(_cntxt),
_properties(p),
_spriteSheet_p(_ss),
_model({
        .x = _properties.positionInWorld.x,
        .y = _properties.positionInWorld.y,
        .w = (float)(_properties.sizeInTiles.x * _ss->getSliceSize().x) / vp->pixel_per_meter,
        .h = (float)(_properties.sizeInTiles.y * _ss->getSliceSize().y) / vp->pixel_per_meter
    },
    _properties.ellasticCoef,
    _properties.frictionCoef
),
_tileMap( _ss->getSliceSize(), p.sizeInTiles, _calculateTileSetFromConfig(), p.tileMapSpriteSliceMatrix ),
_viewport_p(vp)
{}

RectPlatform::~RectPlatform(){
    _spriteSheet_p = nullptr;
    _viewport_p = nullptr;
}

void RectPlatform::render(){

    Vec2D_Int positionCanvas = _viewport_p->viewPortToCanvas( 
        _viewport_p->worldToViewPort( _properties.positionInWorld ) 
    );

    const Vec2D_Int& platformSize = _tileMap.getSizeInTiles();

    for (int _matrix_row = 0; _matrix_row < platformSize.y; _matrix_row++){

        for (int _matrix_col = 0; _matrix_col < platformSize.x; _matrix_col++){

            SpriteSlice slice = _tileMap.getTileAt(platformSize.y - _matrix_row - 1, _matrix_col);

            _spriteSheet_p->renderSlice(
                {
                    .x = positionCanvas.x + _matrix_col * slice.frame.w,
                    .y = positionCanvas.y - _matrix_row * slice.frame.h
                }, 
                slice
            );
        }
    }
}

RectPlatformPhysicsModel *RectPlatform::getPhysicsModel_ptr(){
    return &_model;
}

std::vector<SpriteSlice> RectPlatform::_calculateTileSetFromConfig(){
    
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

