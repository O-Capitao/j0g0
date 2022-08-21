#include <assert.h>

#include "tilemap.hpp"


using namespace j0g0;

TileMap::TileMap( 
    const Vec2D_Int &tileSize,
    const Vec2D_Int &blockSize,
    std::vector<SpriteAnimationSlice> tileSet,
    std::vector<Uint8> sliceMap
){
    assert( tileSet.size() > 0 );

    _tileSize = tileSize;
    _blockSize_inTiles = blockSize;
    _tileSet = tileSet;

    _blockMap_Matrix = new int* [_blockSize_inTiles.y];

    for (int j = 0; j < _blockSize_inTiles.y; j++){
        _blockMap_Matrix[j] = new int[_blockSize_inTiles.x];
    }

    for (int i = 0; i < _blockSize_inTiles.y; i++){

        for (int j = 0; j <  _blockSize_inTiles.x; j++){

            int sliceIndex = (i)* _blockSize_inTiles.x + j;

            _blockMap_Matrix[i][j] = sliceIndex;

        }

    }

}

TileMap::~TileMap(){
    for (int i = 0; i < _blockSize_inTiles.y; i++){
        delete[] _blockMap_Matrix[i];
    }

    delete[] _blockMap_Matrix;

}

void TileMap::setTileAt(int row, int col, int frameIndex ){
    assert( row < _blockSize_inTiles.y && col < _blockSize_inTiles.x && frameIndex < _tileSet.size() );

    _blockMap_Matrix[row][col] = frameIndex;
}

const SpriteAnimationSlice& TileMap::getTileAt(int row, int col){
    assert( row < _blockSize_inTiles.y && col < _blockSize_inTiles.x );

    int sprite_index = _blockMap_Matrix[row][col];
    return _tileSet[sprite_index];
}

void TileMap::setRowOfTiles(int row, int frameIndex){
    assert( row < _blockSize_inTiles.y && frameIndex < _tileSet.size() );

    for (int j = 0; j < _blockSize_inTiles.x; j++){
        _blockMap_Matrix[row][j] = frameIndex;
    }
}

void TileMap::setColumnOfTiles(int col, int frameIndex){
    assert( col < _blockSize_inTiles.x && frameIndex < _tileSet.size() );

    for (int i = 0; i < _blockSize_inTiles.y; i++){
        _blockMap_Matrix[i][col] = frameIndex;
    }
}

void TileMap::setAllTiles(int frameIndex){
    assert( frameIndex < _tileSet.size() );
    
    for (int i = 0; i < _blockSize_inTiles.y; i++){

        for (int j = 0; j < _blockSize_inTiles.x; j++){

            _blockMap_Matrix[i][j] = frameIndex;

        }
    }
}