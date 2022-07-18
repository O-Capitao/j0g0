#pragma once

#include "optional"
#include "SDL.h"
#include "string"

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
        bool FALLING;

        BoxPhysicsModel(){}

        BoxPhysicsModel( 
            float m, float fc,
            const Vec2D_Float &p,
            const FloatRect &b,
            const std::string& ownerId,
            const Vec2D_Float& initialVelocity
        );
        
        void update(float dt_inMillisec);

        // when collision is detected,
        // run this to set the box into the line
        void snapToLine( 
            const StraightFloatLine &line,
            SnappedPointType snappedPoint
        );

        bool checkIfInBounds();

        private:
            std::string _ownerId;
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

        private:
            std::string _ownerId;
        
    };

    struct ObjectToPlatformCollisionPair{
        BoxPhysicsModel *box_p;
        RectPlatformPhysicsModel *plat_p;
        StraightFloatLine platformBoundary;

    };

    struct PlatformGamePhysics {

        // the velocity that, on collision, we will
        // ignore values bellow -> smoother snap to lines while bouncing
        const float VEL_THRESHOLD = 2;
        std::vector<RectPlatformPhysicsModel*> platforms;
        std::vector<BoxPhysicsModel*> objects;

        void resolveModel(Uint32 dt_ms);
        void removeObject(const std::string ownerId);
        // void free();

        private:
            void _checkCollisions();
            std::optional<ObjectToPlatformCollisionPair> _findObstacle( BoxPhysicsModel* obj );
    };
}