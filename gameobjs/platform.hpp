#pragma once

#include "SDL.h"

#include "gameobj.hpp"
#include "graphics/gamegeometry.hpp"
#include "graphics/tilemap.hpp"
#include "graphics/viewport.hpp"


namespace j0g0{

    struct PlatformProperties{

        Vec2D_Float positionInWorld;
        Vec2D_Int sizeInTiles;
        std::string spriteSheetId;
        std::vector<Uint8> tileMapSpriteSliceMatrix;
        // ranging from 0 - 180. For larger values use flip
        std::vector<Uint8> sliceRotations_in90Deg;
        std::vector<bool>sliceFlip_H;
        std::vector<bool>sliceFlip_V;

        std::vector<SpriteSlice> calculateTileSet(SpriteSheet* ss);
    };





    struct Platform : public GameObj{

        Platform( 
            RenderingContext* _context, 
            SpriteSheet* _spriteSheet,
            ViewPort *vp,
            PlatformProperties &properties
        );
        
        ~Platform();

        void render();
        void update(){};

        // void setViewport( ViewPort *vp);

        private:

            static Polygon _createOutline( const Vec2D_Int& blockSizeInTiles, const Vec2D_Int& tileSize, ViewPort *vp );

            SpriteSheet *_spriteSheet_p;
            TileMap _tileMap;
            Polygon _hardOutline;
            
            PlatformProperties _properties;
            ViewPort *_viewport_p;

    };
}