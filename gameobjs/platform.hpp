#pragma once

#include "SDL.h"

#include "gameobj.hpp"
#include "gameconfig.hpp"
#include "graphics/gamegeometry.hpp"
#include "graphics/tilemap.hpp"
#include "graphics/viewport.hpp"
#include "physics/physics.hpp"

namespace j0g0{

    struct RectPlatform : public GameObj{

        RectPlatform( 
            RenderingContext* _context, 
            SpriteSheet* _spriteSheet,
            ViewPort *vp,
            PlatformProperties &properties
        );
        
        ~RectPlatform();

        void render();
        void update(Uint32 dt){};
        RectPlatformPhysicsModel *getPhysicsModel_ptr();
        
        private:
            
            PlatformProperties _properties;
            RectPlatformPhysicsModel _model;
            SpriteSheet *_spriteSheet_p;
            TileMap _tileMap;;
            ViewPort *_viewport_p;

            std::vector<SpriteSlice> _calculateTileSetFromConfig();

    };
}