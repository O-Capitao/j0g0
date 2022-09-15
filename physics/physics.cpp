#include "physics.hpp"
#include "math.h"
#include "game-defines.hpp"

using namespace j0g0;



// GameObjectModel
//
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
GameObjectModel::GameObjectModel(
    const Vec2D_Float &_initialPosition,
    const Vec2D_Float &_size,
    const Vec2D_Float &_initialVelocity,
    const Vec2D_Float &_initialAcceleration,
    const std::string &_ownerObjectId,
    bool _isItStatic,
    bool _isItMassive
)
:_boundingBox({
    .x = _initialPosition.x,
    .y = _initialPosition.y,
    .w = _size.x,
    .h = _size.y
}),
_velocity( _initialVelocity ),
_acceleration( _initialAcceleration ),
_isOnGround( false ),
_ownerId( _ownerObjectId ),
_isStatic( _isItStatic ),
_isMassive( _isItMassive ){

    #if DEBUG
        printf("GameObjectModel has been built for %s\n", _ownerId.c_str());
    #endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
GameObjectModel::~GameObjectModel(){

    #if DEBUG
        printf("Destroyed GameObjectModel %s\n", _ownerId.c_str());
    #endif
}




///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// PlatformModel
//
PlatformModel::PlatformModel(
    const SDL_FRect &bb,
    const std::string &ownerId,
    const Vec2D_Float &v_0,
    const bool &isItStatic
): GameObjectModel(
    {bb.x, bb.y},
    {bb.w, bb.h},
    v_0,
    {0,0},
    ownerId,
    isItStatic,
    true )
{

    #if DEBUG
        printf("PlatformModel has been built for %s\n", _ownerId.c_str());
    #endif

    _lastPosition = {_boundingBox.x, _boundingBox.y};
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
PlatformModel::~PlatformModel(){
    #if DEBUG
        printf("PlatformModel has been destroyed for %s\n", _ownerId.c_str());
    #endif  
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void PlatformModel::update(float dt_s){

    _lastPosition = {_boundingBox.x, _boundingBox.y};
    if (_eligibleForUpdate && !_isStatic && ( _velocity.x != 0 || _velocity.y != 0)){

        _boundingBox.x += _velocity.x * dt_s;
        _boundingBox.y += _velocity.y * dt_s;

    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// ActorModel
//
ActorModel::ActorModel( 
    const Vec2D_Float &_initialPosition,
    const Vec2D_Float &_size,
    const Vec2D_Float &_initialVelocity,
    const Vec2D_Float &_initialAcceleration,
    const std::string &_ownerActorId
):GameObjectModel(
    _initialPosition,
    _size,
    _initialVelocity,
    _initialAcceleration,
    _ownerActorId,
    false,
    false // actors are not massive
){

    _eligibleForUpdate = true;
    _lastPosition = _initialPosition;
    
    #if DEBUG
        printf("ActorModel has been built for %s\n", _ownerId.c_str());
    #endif
}



///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
ActorModel::~ActorModel(){
    #if DEBUG
        printf("ActorModel has been destroyed for %s\n", _ownerId.c_str());
    #endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ActorModel::update(float dt_s){
    
    if (!_eligibleForUpdate){
        return;
    }

    _lastPosition = {
        .x = _boundingBox.x,
        .y = _boundingBox.y
    };

    if (!_isOnGround){

        if ( abs(_velocity.x /2 + _velocity.y/2) < _terminalVelocity ){
            _velocity.x += _acceleration.x * dt_s ;
            _velocity.y += _acceleration.y * dt_s ;
        }

    } else {

        assert( _currentGround != NULL );
        
        const Vec2D_Float &platVelocity = _currentGround->getVelocity();
        
        _velocity.x = platVelocity.x + _velocityX_rel_to_currentGround;
        _velocity.y = platVelocity.y;
    }

    _boundingBox.x += dt_s * _velocity.x;
    _boundingBox.y += dt_s * _velocity.y; 

}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// Called specifically if Actor's state changes from being in the air
// to being set in a platform
void ActorModel::snapToPlatformModel( PlatformModel *tgt_p ){
    
    _isOnGround = true;
    
    assert( _currentGround == NULL );

    // no slide when landing.
    _velocityX_rel_to_currentGround = 0;
    _velocity = tgt_p->getVelocity();

    SDL_FRect _tgtBB = tgt_p->getBoundingBox();
    _boundingBox.y = _tgtBB.y + _tgtBB.h;
    _currentGround = tgt_p;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool ActorModel::checkIfInBounds(){
    return _boundingBox.y > 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ActorModel::setWalkingVelocity(float walkingVelocity){
    if (_isOnGround){
        _velocityX_rel_to_currentGround = walkingVelocity;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ActorModel::jump( const Vec2D_Float &dv ){

    assert( _isOnGround && _currentGround != NULL );
    
    const Vec2D_Float &currentGroundVelocity = _currentGround->getVelocity();

    _velocity.x += ( currentGroundVelocity.x + dv.x );
    _velocity.y += ( currentGroundVelocity.y +  dv.y );

    _isOnGround = false;
    _releaseGround();

    // give this to get some clear space between
    // the jumping Player and the potentially moving platform under him
    // and avoid firing the collision detector.
    _boundingBox.y+=0.1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void ActorModel::_releaseGround(){
    _currentGround = NULL;
    _velocityX_rel_to_currentGround = 0;
    _isOnGround = false;
}



// WorldModel
//
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void WorldModel::solveStep(float dt_s){
    _resolveActorToPlatformInteractions(dt_s);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void WorldModel::_resolveActorToPlatformInteractions(float dt_s){
    
    std::vector<CollisionPair> contacts;

    for (ActorModel *a: actors){

        if (a->_eligibleForUpdate){

            for (PlatformModel *p : platforms){

                // if ( p->_eligibleForUpdate){
                
                // find all platforms that are shown in screen
                if (true){

                    if ( SDL_HasIntersectionF( &(a->getBoundingBox()), &(p->getBoundingBox())) ){
                        // Find another CollisionPair for which A is the present Actor
                        // const std::string &ownerId = a->_ownerId;
                        CollisionPair curr_contact = _resolveCollision( a, p );
                        const std::string &actorId = a->getOwnerId();

                        std::vector<CollisionPair>::iterator i =  std::find_if( 
                            contacts.begin(), contacts.end(),
                            [actorId](const CollisionPair &cp) -> 
                                bool { 
                                    return cp.modelA->getOwnerId().compare( actorId ) == 0; 
                                }
                        );

                        if (i != contacts.end()){
                            const CollisionPair & alreadyIn = *i;
                            if (alreadyIn.dt_collision > curr_contact.dt_collision ){
                                (*i) = curr_contact;
                            }

                        } else {
                            contacts.push_back( curr_contact );
                        }
                    }                    
                }
            }
        }
    }

    // by this point there should be 1 Collision pair for each Actor that collided.
    for (CollisionPair cp : contacts){
        _setActorPositionsAfterCollision(cp, dt_s);
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
void WorldModel::_setActorPositionsAfterCollision(const CollisionPair & cPair, float dt_s){
    
    if (cPair.contactFace_A == RectangeFace::BOTTOM ){
        // // landing..

        assert(cPair.modelA != NULL);
        assert(cPair.modelB != NULL);

        cPair.modelA->snapToPlatformModel(cPair.modelB);

    } else {
        // Simple collision
        
        // A IS THE ONE THAT WILL MOVE,
        // just let it move 
        cPair.modelA->_boundingBox.x = cPair.modelA->_lastPosition.x + cPair.dt_collision * cPair.modelA->_velocity.x;
        cPair.modelA->_boundingBox.y = cPair.modelA->_lastPosition.y + cPair.dt_collision * cPair.modelA->_velocity.y;

        // set appropriate velocity - NO SLIP
        cPair.modelA->setVelocity({0,0});
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
CollisionPair WorldModel::_resolveCollision( ActorModel *A, PlatformModel *B ){
    
    assert(A->_isMassive || B->_isMassive);

    CollisionPair retval;
    retval.modelA = A;
    retval.modelB = B;

    // last position
    const Vec2D_Float &lastPositionA = A->getLastPosition();
    const Vec2D_Float &lastPositionB = B->getLastPosition();

    const Vec2D_Float &vA = A->getVelocity();
    const Vec2D_Float &vB = B->getVelocity();

    const Vec2D_Float vel_A_rel_to_B = {
        .x = vA.x - vB.x,
        .y = vA.y - vB.y
    };

    // bounding Box is at current position
    const SDL_FRect &bbA = A->getBoundingBox();
    const SDL_FRect &bbB = B->getBoundingBox();

    const Vec2D_Float pos_A_rel_to_B_lastPosition = {
        .x = lastPositionA.x - lastPositionB.x,
        .y = lastPositionA.y - lastPositionB.y
    };

    float distanceToAxisA_to_B;

    if(A->_isOnGround){
        // special case for ground-ground contact\


        if ( vel_A_rel_to_B.x > 0){
            retval.contactFace_A = RectangeFace::RIGHT;
            retval.contactFace_B = RectangeFace::LEFT;
            distanceToAxisA_to_B = lastPositionB.x - (lastPositionA.x + bbA.w);
        } else {
            retval.contactFace_A = RectangeFace::LEFT;
            retval.contactFace_B = RectangeFace::RIGHT;
            distanceToAxisA_to_B = (lastPositionB.x + bbB.w) - lastPositionA.x;
        }

        retval.dt_collision = distanceToAxisA_to_B / vA.x;

        // assert(retval.dt_collision > 0);

           return retval;
    }




    /*  
                     2     . 1            . 2
                    _ _ _ _.______________. _ _ _ _
                   1       |     B        | 1
                    _ _ _ _|______________|_ _ _ _ _
                    2      . 1            . 2
                           .              .
     */
    // scenario 1 - Object A was in Areas "1" before the collision -> 1 possible outcome, 
    //              function of position and velocity

    // Object A was above/bellow B -> vertical collision
    if ( pos_A_rel_to_B_lastPosition.x > -bbA.w && pos_A_rel_to_B_lastPosition.x < bbB.w ) {

        if ( vel_A_rel_to_B.y > 0 ){
            retval.contactFace_A = RectangeFace::UP;
            retval.contactFace_B = RectangeFace::BOTTOM;

            distanceToAxisA_to_B = (-pos_A_rel_to_B_lastPosition.y) - bbA.h;
            retval.dt_collision = distanceToAxisA_to_B / vel_A_rel_to_B.y; 
            
        } else {
            retval.contactFace_A = RectangeFace::BOTTOM;
            retval.contactFace_B = RectangeFace::UP;

            distanceToAxisA_to_B = pos_A_rel_to_B_lastPosition.y - bbB.h;
            retval.dt_collision = distanceToAxisA_to_B / vel_A_rel_to_B.y;
        }
    // Object A was left/right from B -> horizntal collision
    } else if ( pos_A_rel_to_B_lastPosition.y > - bbA.h && pos_A_rel_to_B_lastPosition.y < bbB.h ){

        if ( vel_A_rel_to_B.x > 0 ){
            retval.contactFace_A = RectangeFace::RIGHT;
            retval.contactFace_B = RectangeFace::LEFT;

            distanceToAxisA_to_B = (-pos_A_rel_to_B_lastPosition.x) - bbA.w;
            retval.dt_collision = distanceToAxisA_to_B / vel_A_rel_to_B.x;
        } else {
            retval.contactFace_A = RectangeFace::LEFT;
            retval.contactFace_B = RectangeFace::RIGHT;

            distanceToAxisA_to_B = pos_A_rel_to_B_lastPosition.x - bbB.w;
            retval.dt_collision = distanceToAxisA_to_B / vel_A_rel_to_B.x;
        }
    // scenario 2 - Oject A in Areas "2"
    // Ambiguity Must Be solved    
    } else {
        // upper left
        if (pos_A_rel_to_B_lastPosition.x < -bbA.w && pos_A_rel_to_B_lastPosition.y > bbB.h ){

            float distanceToVerticalColl = pos_A_rel_to_B_lastPosition.y - bbB.h;
            float distanceToHorizontalColl = (-pos_A_rel_to_B_lastPosition.x) - bbA.w;

            float dtVertCollision = distanceToVerticalColl / vel_A_rel_to_B.y;
            float dtHorCollision = distanceToHorizontalColl / vel_A_rel_to_B.x;

            if (dtVertCollision < dtHorCollision){

                retval.dt_collision = dtVertCollision;
                retval.contactFace_A = RectangeFace::BOTTOM;
                retval.contactFace_B = RectangeFace::UP;

            } else {
                retval.dt_collision = dtHorCollision;
                retval.contactFace_A = RectangeFace::RIGHT;
                retval.contactFace_B = RectangeFace::LEFT;
            }

        }
        // bottom left  
        else if ( pos_A_rel_to_B_lastPosition.x < -bbA.w && pos_A_rel_to_B_lastPosition.y < -bbA.h  ){
            
            float distanceToVerticalColl = (-pos_A_rel_to_B_lastPosition.y) - bbA.h;
            float distanceToHorizontalColl = (-pos_A_rel_to_B_lastPosition.x) - bbA.w;

            float dtVertCollision = distanceToVerticalColl / vel_A_rel_to_B.y;
            float dtHorCollision = distanceToHorizontalColl / vel_A_rel_to_B.x;

            if (dtVertCollision < dtHorCollision){

                retval.dt_collision = dtVertCollision;
                retval.contactFace_A = RectangeFace::UP;
                retval.contactFace_B = RectangeFace::BOTTOM;

            } else {
                retval.dt_collision = dtHorCollision;
                retval.contactFace_A = RectangeFace::RIGHT;
                retval.contactFace_B = RectangeFace::LEFT;
            }

        }
        // top right
        else if ( pos_A_rel_to_B_lastPosition.x > bbB.w && pos_A_rel_to_B_lastPosition.y > bbB.h ){
            float distanceToVerticalColl = pos_A_rel_to_B_lastPosition.y - bbB.h;
            float distanceToHorizontalColl = pos_A_rel_to_B_lastPosition.x - bbB.w;

            float dtVertCollision = distanceToVerticalColl / vel_A_rel_to_B.y;
            float dtHorCollision = distanceToHorizontalColl / vel_A_rel_to_B.x;

            if (dtVertCollision < dtHorCollision){

                retval.dt_collision = dtVertCollision;
                retval.contactFace_A = RectangeFace::BOTTOM;
                retval.contactFace_B = RectangeFace::UP;

            } else {
                retval.dt_collision = dtHorCollision;
                retval.contactFace_A = RectangeFace::LEFT;
                retval.contactFace_B = RectangeFace::RIGHT;
            }
        // bottom right
        } else if ( pos_A_rel_to_B_lastPosition.x > bbB.w && pos_A_rel_to_B_lastPosition.y < -bbA.h ){
            float distanceToVerticalColl = (-pos_A_rel_to_B_lastPosition.y) - bbA.h;
            float distanceToHorizontalColl = pos_A_rel_to_B_lastPosition.x - bbB.w;

            float dtVertCollision = distanceToVerticalColl / vel_A_rel_to_B.y;
            float dtHorCollision = distanceToHorizontalColl / vel_A_rel_to_B.x;

            if (dtVertCollision < dtHorCollision){

                retval.dt_collision = dtVertCollision;
                retval.contactFace_A = RectangeFace::UP;
                retval.contactFace_B = RectangeFace::BOTTOM;

            } else {
                retval.dt_collision = dtHorCollision;
                retval.contactFace_A = RectangeFace::LEFT;
                retval.contactFace_B = RectangeFace::RIGHT;
            }
        } else {
            throw std::runtime_error("Invalid Collision state\n;");
        }
    }


    return retval;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
void WorldModel::removeObject( const std::string id ){
    for ( auto it = actors.begin(); it != actors.end(); ){
        if ((*it)->_ownerId == id){
        //    delete * it;
           it = actors.erase(it); 
        } else{
            ++it;
        }
    }
}
