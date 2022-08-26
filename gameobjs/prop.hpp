#pragma once

#include "gameobjs/gameobj.hpp"
#include "graphics/rendering.hpp"
#include "graphics/viewport.hpp"
#include "graphics/spritesheet.hpp"
#include "graphics/tilemap.hpp"
#include "gameconfig.hpp"

namespace j0g0 {
    // Holds data representing content that is inserted
    // into the World, that has no possibility of interaction - presentation only

    // Base class is for static prop. a single frame.
    struct Prop: public GameObj{

        Prop(
            RenderingContext* _context, 
            SpriteSheet* _spriteSheet,
            ViewPort *vp,
            PropConfig &properties
        );

        void render();
        void update(float dt_s){};
        static bool compareForSort( const Prop &p1, const Prop &p2);

        protected:
            PropConfig _config;
            FloatRect _box;
            SpriteSheet *_spriteSheet_p;
            ViewPort *_viewPort_p;

        private:
            bool _isInViewPort();
    };

    struct AnimatedProp: public Prop{
        AnimatedProp(
            RenderingContext* _context, 
            SpriteSheet* _spriteSheet,
            ViewPort *vp,
            PropConfig &properties    
        );

        void update(float dt_s);

        private:
            SpriteAnimation animation;
            float _internalClock;
    };

    struct TiledProp: public Prop{

    };
}