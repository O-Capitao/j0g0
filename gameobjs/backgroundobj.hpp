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
            BackgroundConfig &properties
        );

        void render();
        void update(float dt_s);

        private:
            BackgroundConfig _properties;
            SpriteSheet *_spriteSheet_p;
            TileMap _tileMap;
            ViewPort *_viewport_p;
            
            Vec2D_Int _positionInCanvas;
            Vec2D_Float _previousVP_position_World;

            // Note - this is a duplicate of the
            // Plaform function of same name. TODO: Get it in a single place.
            std::vector<SpriteAnimationSlice> _calculateTileSetFromConfig();
            void _renderSingleBackground(const Vec2D_Int& background_top_left_corner);
    };
}