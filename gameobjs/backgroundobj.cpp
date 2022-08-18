#include "backgroundobj.hpp"

using namespace j0g0;

BackgroundObj::BackgroundObj(
    RenderingContext* _cntxt, 
    SpriteSheet* _ss, 
    ViewPort* vp,
    BackgroundProperties &p
):GameObj(_cntxt),
_properties(p),
_spriteSheet_p(_ss),
_tileMap( _ss->getSliceSize(), p.sizeInTiles, _calculateTileSetFromConfig(), p.tileMapSpriteSliceMatrix ),
_viewport_p(vp),
_positionInCanvas(p.positionInCanvas)
{}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
BackgroundObj::~BackgroundObj(){
    _spriteSheet_p = nullptr;
    _viewport_p = nullptr;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Duplicate Logic in Platform.
///////////////////////////////////////////////////////////////////////////////////////////////////
void BackgroundObj::render(){

    const Vec2D_Int& blockSize = _tileMap.getSizeInTiles();

    for (int _matrix_row = 0; _matrix_row < blockSize.y; _matrix_row++){

        for (int _matrix_col = 0; _matrix_col < blockSize.x; _matrix_col++){

            SpriteSlice slice = _tileMap.getTileAt(blockSize.y - _matrix_row - 1, _matrix_col);

            _spriteSheet_p->renderSlice(
                {
                    .x = _positionInCanvas.x + _matrix_col * slice.frame.w,
                    .y = _positionInCanvas.y - (_matrix_row + 1) * slice.frame.h
                }, 
                slice,
                false
            );
        }
    }
}

void BackgroundObj::update(float dt_s){

}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Necessary for initting of the TileMap.
// TODO: Fix: Duplicate of the method with same name in Platform
//
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<SpriteSlice> BackgroundObj::_calculateTileSetFromConfig(){
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