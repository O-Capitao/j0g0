#pragma once

#include "optional"

#include "graphics/gamegeometry.hpp"

namespace j0g0 {

    enum TrajectoryType {
        PROJECTILE,
        PROJECTILE_AND_GROUND_EFFECT,
        PARACHUTE,

    };

    enum SnappedPointType {
        BOTTOM_LEFT, 
        BOTTOM_RIGHT, 
        TOP_RIGHT, 
        TOP_LEFT,
        BOTTOM_CENTER
    };

    struct BoxPhysicsModel {
        
        friend struct PlatformGamePhysics;

        FloatRect box;
        Vec2D_Float positionInWorld;
        bool FALLING;

        BoxPhysicsModel(){}

        BoxPhysicsModel( 
            float m, float fc,
            const Vec2D_Float &p,
            const FloatRect &b
        );
        
        void update(float dt_inMillisec);

        // when collision is detected,
        // run this to set the box into the line
        void snapToLine( 
            const StraightFloatLine &line,
            SnappedPointType snappedPoint
        );

        private:
    
            float _mass;
            float _frictionCoef;
            float _terminalVelocity;
            TrajectoryType _fallingMode;
            Vec2D_Float _velocity;
            Vec2D_Float _acceleration;
            StraightFloatLine *_currentGround;
            Vec2D_Float _lastDisplacement;

    };

    struct RectPlatformPhysicsModel{
        
        friend struct PlatformGamePhysics;

        RectPlatformPhysicsModel(
            const FloatRect &bounding_box,
            float ellastic_coef,
            float friction_coef
        );

        friend struct PlatformGamePhysics;

        FloatRect box;

        // defined up-left-down-right
        StraightFloatLine borders[4];
        const float frictionCoef, ellasticCoef;
        
    };

    struct PlatformGamePhysics {
        std::vector<RectPlatformPhysicsModel*> platforms;
        std::vector<BoxPhysicsModel*> objects;

        void resolveModel(float dt);
        // void free();

        private:
            void _checkCollisions();
            std::optional<StraightFloatLine> _findObstacle( BoxPhysicsModel* obj );
    };
}