#include <iostream>
#include <exception>

#include "tilemapthing.hpp"

using namespace j0g0;

/**
* Tile Block
* 
*/
TileBlock::TileBlock( const Vec2D_Float& _block_position, const Vec2D_Int& _block_size ){

    position = _block_position;
    blockSize = _block_size;

    blockMap = new int* [blockSize.y];

    for (int j = 0; j < blockSize.y; j++){
        blockMap[j] = new int[blockSize.x];
    }

    setAllTiles(MAX_TILESET_SIZE+1);
}

TileBlock::~TileBlock(){

    for (int i = 0; i < blockSize.y; i++){
        delete[] blockMap[i];
    }

    delete[] blockMap;

}

TileBlock TileBlock::operator=(const TileBlock &tileBlock){
    
    TileBlock newTileBlock( tileBlock.position, tileBlock.blockSize );

    // copy block map
    for (int i = 0; i < tileBlock.blockSize.y; i++){
        for (int j = 0; j < tileBlock.blockSize.x; j++){
            newTileBlock.blockMap[i][j] = tileBlock.blockMap[i][j];
        }
    }

    return newTileBlock;
}

void TileBlock::setTileAt(int row, int col, int frameIndex ){
    blockMap[row][col] = frameIndex;
}

void TileBlock::setRowOfTiles(int row, int frameIndex){
    for (int j = 0; j < blockSize.x; j++){
        blockMap[row][j] = frameIndex;
    }
}

void TileBlock::setColumnOfTiles(int col, int frameIndex){
    for (int i = 0; i < blockSize.y; i++){
        blockMap[i][col] = frameIndex;
    }
}

void TileBlock::setAllTiles(int frameIndex){

    for (int i = 0; i < blockSize.y; i++){

        for (int j = 0; j < blockSize.x; j++){

            blockMap[i][j] = frameIndex;

        }
    }
}





/**
 * 
 * TileMapThing
 */
TileMapThing::TileMapThing( 
    RenderingContext* _context,
    std::string path_to_img,
    int n_frames,
    int x_divisions,
    int y_divisions,
    ViewPort* _vp_p
) 
: GameThing(_context, {0,0,0,0} ),spriteSheet(SpriteSheet(_context)),tileBlock( {0, 0}, {0,0} ) {

    spriteSheet.initSpriteSheet( path_to_img, x_divisions, y_divisions );
    viewPort_p = _vp_p;

}

TileMapThing::~TileMapThing(){
    
    // for (auto p : tileBlocks ){
    //     delete p;
    //     p = NULL;
    // }

    // tileBlocks.empty();
    viewPort_p = NULL;

}

void TileMapThing::createTileBlock(Vec2D_Float& position, Vec2D_Int& size){

    TileBlock _block(position, size);
    tileBlock = _block;
    
}

void TileMapThing::render(){

    Vec2D_Int _block_position_in_canvas =  viewPort_p->viewPortToCanvas( viewPort_p->worldToViewPort(  tileBlock.position ));

    for (int _matrix_row = 0; _matrix_row < tileBlock.blockSize.y; _matrix_row++){

        for (int _matrix_col = 0; _matrix_col < tileBlock.blockSize.x; _matrix_col++){

            int _tileValue = tileBlock.blockMap[_matrix_row][_matrix_col];

            // The portion of the SpriteSheet that is represented by the _tileValue
            SpriteFrame _slice = spriteSheet.getFrameAt(_tileValue);//tileSet.getSpriteFromTileSet( _tileValue );

            // TODO - alidations?
            SDL_Rect& src = _slice.frame;

            // target -> 
            SDL_Rect tgt = {
                .x = _block_position_in_canvas.x + _matrix_col * _slice.frame.w,
                .y = _block_position_in_canvas.y + _matrix_row * _slice.frame.h,
                .w = _slice.frame.w,
                .h = _slice.frame.h
            };

            SDL_RenderCopy( 
                context->renderer, 
                spriteSheet.texture, 
                &src, &tgt
            );

        }
    }
}

