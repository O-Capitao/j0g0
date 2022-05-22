#pragma once

#include <vector>
#include <string>

#include "SDL.h"

#include "graphics/gamegeometry.hpp"
#include "graphics/spritesheet.hpp"
#include "graphics/rendering.hpp"

#include "gamething.hpp"

namespace j0g0 {

    struct TileBlock {

        TileBlock( const Vec2D_Float& _block_position, const Vec2D_Int& _block_size );
        ~TileBlock();
        TileBlock operator=(const TileBlock &tileBlock);

        const int MAX_TILESET_SIZE = 99;
        /* 
        * Matrix mapping each tile in the block
        * to the corresponding Sprite Slice.
        */
        int** blockMap;

        /* Size in Tiles */
        Vec2D_Int blockSize;

        /* Position in World */
        Vec2D_Float position;

        void setTileAt(int row, int col, int frameIndex);
        void setAllTiles(int frameIndex);
        void setRowOfTiles(int row, int frameIndex);
        void setColumnOfTiles(int col, int frameIndex);
              
    };






    struct TileMapThing: public GameThing {

        TileMapThing( RenderingContext* _context,
            std::string path_to_img,
            int n_frames,
            int x_divisions,
            int y_divisions,
            ViewPort* _viewPort_p
        );

        ~TileMapThing();

        void createTileBlock(Vec2D_Float& position, Vec2D_Int& size);
        
        void render();
        void update(){}

        private:

            SpriteSheet spriteSheet;
            ViewPort* viewPort_p;
            TileBlock tileBlock;
        
    };      
}