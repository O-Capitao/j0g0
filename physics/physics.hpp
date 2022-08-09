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
        
        // make the overall physics runtime
        // a friend of this class
        friend struct PlatformGamePhysics;

        
        FloatRect box;
        bool FALLING;
        // use it for Actors with
        // precise landing - e.g: Player
        bool isBounceable = true;

        BoxPhysicsModel(){}

        BoxPhysicsModel( 
            float m, float fc,
            const Vec2D_Float &p,
            const FloatRect &b,
            const std::string& ownerId,
            const Vec2D_Float& initialVelocity,
            bool _isBounceable
        );
        
        void update(float dt_inMillisec);

        // when collision is detected,
        // run this to set the box into the line
        void snapToLine( 
            StraightFloatLine* line,
            SnappedPointType snappedPoint,
            Vec2D_Float *platVelocity_p
        );

        bool checkIfInBounds();

        void applyDv(const Vec2D_Float &dv);

        void setWalkingVelocity(float walkingVelocity){ _velocity.x = walkingVelocity; }

        const Vec2D_Float& getVelocity(){
            return _velocity;
        }

        void releaseGround(){
            _currentGroundLine_p = NULL;
            _currentGroundVelocity_p = NULL;
        }

        private:
            std::string _ownerId;
            float _mass;
            float _frictionCoef;
            float _terminalVelocity;
            TrajectoryType _fallingMode;

            // absolute velocity
            Vec2D_Float _velocity;
            Vec2D_Float _acceleration;

            // About the current Ground
            StraightFloatLine *_currentGroundLine_p;
            Vec2D_Float *_currentGroundVelocity_p;
            
            Vec2D_Float _lastDisplacement;


    };

    struct RectPlatformPhysicsModel{
        
        friend struct PlatformGamePhysics;

        RectPlatformPhysicsModel(
            const FloatRect &bounding_box,
            float ellastic_coef,
            float friction_coef,
            const std::string &ownerId
        );

        friend struct PlatformGamePhysics;

        FloatRect box;

        // defined up-left-down-right
        StraightFloatLine bordersShiftedToOrigin[4];
        StraightFloatLine instantBorders[4];
        
        const float frictionCoef, ellasticCoef;

        Vec2D_Float velocity;
        Vec2D_Float acceleration;

        // run this after updating the box position.
        void initBorders();
        void updateBorders();

        void update(float dt_s);
        void setVelocity(const Vec2D_Float &_newVelocity);

        private:
            std::string _ownerId;
        
    };

    struct ObjectToPlatformCollisionPair{
        BoxPhysicsModel *box_p;
        RectPlatformPhysicsModel *plat_p;
        StraightFloatLine *platformBoundary;

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