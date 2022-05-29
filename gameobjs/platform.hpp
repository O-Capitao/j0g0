#pragma once

#include "SDL.h"

#include "gameobj.hpp"
#include "graphics/gamegeometry.hpp"
#include "graphics/tilemap.hpp"
#include "graphics/viewport.hpp"


namespace j0g0{

    struct Platform : public GameObj{

        Platform( 
            RenderingContext* _context, 
            SpriteSheet* _spriteSheet, 
            const Vec2D_Int& blockSizeInTiles,
            std::vector<SpriteSlice> tileSet,
            const Vec2D_Float& positionInWorld,
            ViewPort *vp
        );
        
        ~Platform();

        void render();
        void update(){};

        void setViewport( ViewPort *vp);

        



        private:

            static Polygon _createOutline( const Vec2D_Int& blockSizeInTiles, const Vec2D_Int& tileSize, ViewPort *vp );

            SpriteSheet *_spriteSheet_p;
            TileMap _tileMap;
            Polygon _hardOutline;
            Vec2D_Float _positionInWorld;
            ViewPort *_viewport_p;

    };

}