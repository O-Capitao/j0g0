#include "backgroundobj.hpp"

using namespace j0g0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
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
_positionInCanvas(p.positionInCanvas),
_previousVP_position_World( vp->positionInWorld )
{}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
BackgroundObj::~BackgroundObj(){
    _spriteSheet_p = nullptr;
    _viewport_p = nullptr;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void BackgroundObj::render(){

    if (_properties.repeatX){
        // tile the background over the entire screen X direction.
        const Vec2D_Int& blockSize = _tileMap.getSizeInTiles();
        const Vec2D_Int& singleBlockSize = _spriteSheet_p->getSliceSize();

        // how many images will fill the screen?
        int image_width = blockSize.x * singleBlockSize.x;
        float images_to_fill_canvas = (float)_viewport_p->canvasSize.x / (float)image_width;

        int images_to_render = (int)images_to_fill_canvas + 1;
        int starting_x;

        // assuming that at all times we have the 'original'
        // background image at max one width away from one of the
        // edges of the canvas -> This is enforced on update call
        //
        //      Find number of images between "original" and left
        //      edge of canvas
        if (_positionInCanvas.x > 0){
            int betweenPositionAndLeftOfCanvas = ceil( (float)_positionInCanvas.x / (float)image_width );
            starting_x = _positionInCanvas.x - betweenPositionAndLeftOfCanvas * image_width;
        } else {
            starting_x = _positionInCanvas.x;
        }

        // render the needed images in a cycle
        for (int x_cursor = starting_x; x_cursor <= (images_to_render * image_width); x_cursor += image_width){
            Vec2D_Int currentCanvasPosition = {
                .x = x_cursor,
                .y = _positionInCanvas.y
            };

            _renderSingleBackground(currentCanvasPosition);
        }

    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void BackgroundObj::update(float dt_s){
    // for now - only scroling in X.
    if (_previousVP_position_World.x != _viewport_p->positionInWorld.x){
        // The VP has moved and the background will move also
        if (_viewport_p->positionInWorld.x == 0){
            _positionInCanvas = _properties.positionInCanvas;
        } else {

            float dx = _viewport_p->positionInWorld.x - _previousVP_position_World.x;
            float dx_canvas = _viewport_p->pixel_per_meter * dx;
            int dx_backg = dx_canvas > 0 ? 
                floor( _properties.ratioToViewportVelocity_pixelPerSec.x * dx_canvas )
                : ceil( _properties.ratioToViewportVelocity_pixelPerSec.x * dx_canvas );

            if (dx_backg != 0){
                _previousVP_position_World = {
                    .x = _viewport_p->positionInWorld.x,
                    .y = _viewport_p->positionInWorld.y
                };
                _positionInCanvas.x -= dx_backg;
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Necessary for initting of the TileMap.
// TODO: Fix: Duplicate of the method with same name in Platform
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<SpriteAnimationSlice> BackgroundObj::_calculateTileSetFromConfig(){
    assert( 
        _properties.tileMapSpriteSliceMatrix.size() == _properties.sliceRotations_in90Deg.size() 
        && _properties.sliceRotations_in90Deg.size() == _properties.sliceFlip_H.size()
        && _properties.sliceFlip_H.size() == (_properties.sizeInTiles.x * _properties.sizeInTiles.y)
    );

    std::vector<SpriteAnimationSlice> slices;

    for (int i = 0; i < (_properties.sizeInTiles.x * _properties.sizeInTiles.y); i++){

        SDL_RendererFlip i_flip = _properties.sliceFlip_V[i] ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;

        if (_properties.sliceFlip_H[i]){
            i_flip = SDL_FLIP_HORIZONTAL;
        }

        // TODO - rotation stuff
        SpriteAnimationSlice i_slice = {
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void BackgroundObj::_renderSingleBackground(const Vec2D_Int& background_top_left_corner){
    const Vec2D_Int& blockSize = _tileMap.getSizeInTiles();

    for (int _matrix_row = 0; _matrix_row < blockSize.y; _matrix_row++){

        for (int _matrix_col = 0; _matrix_col < blockSize.x; _matrix_col++){

            SpriteAnimationSlice slice = _tileMap.getTileAt(blockSize.y - _matrix_row - 1, _matrix_col);

            _spriteSheet_p->renderSlice(
                {
                    .x = background_top_left_corner.x + _matrix_col * slice.frame.w,
                    .y = background_top_left_corner.y  + _matrix_row * slice.frame.h
                }, 
                slice,
                false
            );
        }
    }
}