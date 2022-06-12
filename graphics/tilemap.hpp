#pragma once

#include <vector>

#include "spritesheet.hpp"
#include "gamegeometry.hpp"


namespace j0g0{

    struct TileMap {

        TileMap( 
            const Vec2D_Int &tileSize,
            const Vec2D_Int &blockSize,
            std::vector<SpriteSlice> tileSet,
            std::vector<Uint8> sliceMap
        );

        ~TileMap();

        void setTileAt(int row, int col, int frameIndex);
        const SpriteSlice& getTileAt(int row, int col);
        void setAllTiles(int frameIndex);
        void setRowOfTiles(int row, int frameIndex);
        void setColumnOfTiles(int col, int frameIndex);
        
        const Vec2D_Int& getSizeInTiles(){ return _blockSize_inTiles; };

        private:
        
            Vec2D_Int _tileSize;
            Vec2D_Int _blockSize_inTiles;

            // contem indices referentes
            // ao Vector _tileSet
            int** _blockMap_Matrix;

            std::vector<SpriteSlice> _tileSet;

    };

}