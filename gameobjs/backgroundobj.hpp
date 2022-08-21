#pragma once

#include "gameobj.hpp"

#include "game-defines.hpp"
#include "gameconfig.hpp"
#include "graphics/gamegeometry.hpp"
#include "graphics/tilemap.hpp"
#include "graphics/viewport.hpp"



namespace j0g0{

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Game Object that draws Stationary or Moving Backgrounds
    //
    ///////////////////////////////////////////////////////////////////////////////////////////////
    struct BackgroundObj: public GameObj{

        BackgroundObj(
            RenderingContext* _context,
            SpriteSheet* _spriteSheet,
            ViewPort *vp,
            BackgroundProperties &properties
        );

        ~BackgroundObj();

        void render();
        void update(float dt_s);

        private:
            BackgroundProperties _properties;
            SpriteSheet *_spriteSheet_p;
            TileMap _tileMap;
            ViewPort *_viewport_p;
            
            Vec2D_Int _positionInCanvas;

            // Note - this is a duplicate of the
            // Plaform function of same name. TODO: Get it in a single place.
            std::vector<SpriteAnimationSlice> _calculateTileSetFromConfig();
    };
}