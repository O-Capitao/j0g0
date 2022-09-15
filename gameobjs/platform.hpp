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
            PlatformConfig &properties
        );
        
        void render();
        // no impl for now
        void update(float dt_s){};
        PlatformModel *getPhysicsModel_ptr();
        
        protected:
            PlatformConfig _properties;
            PlatformModel _model;

        private:
            SpriteSheet *_spriteSheet_p;
            
            TileMap _tileMap;
            ViewPort *_viewport_p;

            std::vector<SpriteAnimationSlice> _calculateTileSetFromConfig();

            bool _isVisible();

            // calculate the filled area present in the
            // current ViewPort - called in each render call
            SDL_FRect _calculate_RenderedFillArea();

            // calculate the filled area in the "physical" world
            // called once AFTTER physcs model init -> affects "box" dimensions
            SDL_FRect _calculate_WorldTotalArea();

            // Applies the pixel offset to platorms
            void _applyOffsetsToFillArea(SDL_Rect& rect);
    };


    struct MovingRectPlatformKeyFrame {
        Vec2D_Float p;
        float t_arrival_s;
        float t_wait_s;
        static bool compare( const MovingRectPlatformKeyFrame &o1,  const MovingRectPlatformKeyFrame &o2);
    };

    struct MovingRectPlatform: public RectPlatform {

        MovingRectPlatform(
            RenderingContext* _context, 
            SpriteSheet* _spriteSheet,
            ViewPort *vp,
            PlatformConfig &properties
        );

        void update(float dt_s);

        private:
            std::vector<MovingRectPlatformKeyFrame> _keyFrames;
            std::vector<MovingRectPlatformKeyFrame> _getKeyFramesFromConfig(PlatformConfig &properties);
            Vec2D_Float _findPositionAtTime( float t_s );

            float _totaCycleDuration_s;
            float _currentCycleTime_s;
            int _currentKeyFrame_Index;
    };
}