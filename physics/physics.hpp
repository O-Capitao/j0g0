#pragma once

#include "SDL.h"
#include "string"

#include "graphics/gamegeometry.hpp"

namespace j0g0 {

    /***
     *  Objects used in in-game physics calculations
     *  --------------------
     *  
     *  based on Axis Aligned Bounding Boxes.
     */
    enum RectangeFace{
        LEFT, BOTTOM, RIGHT, UP
    };
    
    // Base class for GameObject physics model.
    // Must be xtended - updated() is virtual
    class GameObjectModel {
        friend class WorldModel;
        protected:
            SDL_FRect _boundingBox;
            Vec2D_Float _lastPosition;
            Vec2D_Float _velocity;
            Vec2D_Float _acceleration;

            std::string _ownerId;

            // is it on top of a Platform?
            bool _isOnGround;
            // can it move?
            bool _isStatic;
            // when one of the objcts in a collision
            // is massive, then it will not move in result of said collision
            // e.g.: an Ator collides with a Platform
            bool _isMassive;

            bool _eligibleForUpdate = false;

        public:

            GameObjectModel(
                const Vec2D_Float &_initialPosition,
                const Vec2D_Float &_size,
                const Vec2D_Float &_initialVelocity,
                const Vec2D_Float &_initialAcceleration,
                const std::string &_ownerObjectId,
                bool _isItStatic,
                bool _isMassive
            );

            ~GameObjectModel();

            // Getters and Setters
            const Vec2D_Float& getVelocity(){ return this->_velocity; }
            const std::string& getOwnerId(){ return this->_ownerId; }
            const SDL_FRect& getBoundingBox(){ return this->_boundingBox; }
            const SDL_FRect *getBoundingBox_Ptr(){ return &(this->_boundingBox); }
            const Vec2D_Float &getLastPosition(){ return this->_lastPosition; }
            const bool &isItOnGround(){ return this->_isOnGround; }
            
            void setVelocity(const Vec2D_Float &_newVelocity){ this->_velocity = _newVelocity; }

            // must be extended
            virtual void update(float dt_s) = 0;

            

    };


    



    class PlatformModel : GameObjectModel {

        friend class WorldModel;
        
        public:

            PlatformModel(
                const SDL_FRect &bounding_box,
                const std::string &ownerId,
                const Vec2D_Float &initialVelocity,
                const bool &isItStatic
            );

            ~PlatformModel();

            void update(float dt_s); 

            // Getters
            const Vec2D_Float& getVelocity(){ return this->_velocity; }
            const std::string& getOwnerId(){ return this->_ownerId; }
            const SDL_FRect& getBoundingBox(){ return this->_boundingBox; }
        
            // Setters
            void setVelocity( const Vec2D_Float &_newVelocity ){ this->_velocity = _newVelocity; }
            void setBoundingBox( const SDL_FRect &newBb ){ this->_boundingBox = newBb; }
    };


    class ActorModel : public GameObjectModel {

        // make the overall physics runtime
        // a friend of this class
        friend class WorldModel;
        // determines bounciness
        float _impactAbsortionCoef = 0;
        // max velocity this Actor can go to.
        float _terminalVelocity = 50;
        PlatformModel *_currentGround = NULL;
        float _velocityX_rel_to_currentGround = 0;
        // Used for collision resolution
        Vec2D_Float _lastPosition = {0,0};

        void _releaseGround();

        public:

            ActorModel( 
                const Vec2D_Float &_initialPosition,
                const Vec2D_Float &_size,
                const Vec2D_Float &_initialVelocity,
                const Vec2D_Float &_initialAcceleration,
                const std::string &_ownerActorId
            );

            ~ActorModel();
            
            void update(float dt_s);
            // when collision is detected with the ground, 
            // run this to set the box into the appropriate line
            void snapToPlatformModel( PlatformModel *targetPlatform );
            bool checkIfInBounds();
            void jump(const Vec2D_Float &dv);
            void setWalkingVelocity(float walkingVelocity);
            
            // GETTER
            const Vec2D_Float& getVelocity(){ return this->_velocity; }
            const std::string& getOwnerId(){ return this->_ownerId; }
            const Vec2D_Float& getLastPosition(){ return this->_lastPosition; }
            
    };

    class CollisionPair{
        public:

            // the 2 models coliding
            ActorModel *modelA = NULL;
            PlatformModel *modelB = NULL;

            // contact faces
            RectangeFace contactFace_A, contactFace_B;
            
            // time since previous step until collision
            // so must be lower than ful dt
            float dt_collision;
    };

    class WorldModel {
        
        public:

            // the velocity that, on collision, we will
            // ignore values bellow -> smoother snap to lines while bouncing
            const float VEL_THRESHOLD = 0.2;
            
            // all of the objects in a single world
            std::vector<PlatformModel*> platforms;
            std::vector<ActorModel*> actors;

            void removeObject( const std::string ownerId );
            void solveStep( float dt_s );

        private:
            void _resolveActorToPlatformInteractions( float dt_s );
            CollisionPair _resolveCollision( ActorModel *A, PlatformModel *B );
            void _setActorPositionsAfterCollision( const CollisionPair & cPair, float dt_s );

    };
}