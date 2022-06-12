#pragma once

#include "SDL.h"

#include "gameobj.hpp"
#include "gameconfig.hpp"
#include "graphics/gamegeometry.hpp"
#include "graphics/tilemap.hpp"
#include "graphics/viewport.hpp"


namespace j0g0{

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

        private:
            
            PlatformProperties _properties;
            static Polygon _createOutline( const Vec2D_Int& blockSizeInTiles, const Vec2D_Int& tileSize, ViewPort *vp );
            SpriteSheet *_spriteSheet_p;
            TileMap _tileMap;
            Polygon _hardOutline;
            
            
            ViewPort *_viewport_p;

            std::vector<SpriteSlice> _calculateTileSetFromConfig();

    };
}