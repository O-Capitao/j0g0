#include "platform.hpp"
#include "game-defines.hpp"

using namespace j0g0;






///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
RectPlatform::RectPlatform( 
    RenderingContext* _cntxt, 
    SpriteSheet* _ss, 
    ViewPort* vp,
    PlatformConfig &p
)
:GameObj(_cntxt),
_properties(p),
_model({
        .x = _properties.positionInWorld.x,
        .y = _properties.positionInWorld.y,
        .w = (float)(_properties.sizeInTiles.x * _ss->getSliceSize().x) / vp->pixel_per_meter,
        .h = (float)(_properties.sizeInTiles.y * _ss->getSliceSize().y) / vp->pixel_per_meter
    },
    p.id,
    {0,0},
    !_properties.isMovingPlatform
),
_spriteSheet_p(_ss),
_tileMap( _ss->getSliceSize(), p.sizeInTiles, _calculateTileSetFromConfig(), p.tileMapSpriteSliceMatrix ),
_viewport_p(vp)
{

    // If a fill direction is specified in the config.
    if (_properties.fillDirection.x != 0 || _properties.fillDirection.y!=0){
        // Alter the bounding box to include the filled area
        _model.setBoundingBox( _calculate_WorldTotalArea() );
    }


    #if DEBUG
    printf("Build RectPlatform: %s\n", _properties.id.c_str());
    #endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void RectPlatform::render(){

    if (!_isVisible()){
        return;
    }

    const SDL_FRect &modelBox = _model.getBoundingBox();

    // render entire color fill before even if larger than
    // viewport - the naive approach - SDL will optimize
    if ( _properties.fillDirection.y < 0 ){

        SDL_Rect fill_rect = _viewport_p->transformRect_Viewport2Canvas(
            _viewport_p->transformRect_World2Viewport(
                modelBox
        ));
        
        _applyOffsetsToFillArea(fill_rect);

        SDL_SetRenderDrawColor(context->renderer, 
            _properties.fillColor.r, 
            _properties.fillColor.g, 
            _properties.fillColor.b, 
            _properties.fillColor.a 
        );

        SDL_RenderFillRect(context->renderer, &fill_rect );
    }

    // the position of the Tiled Portion
    // that needs to be rendered from the SpriteSheet
    Vec2D_Int positionCanvas;
    
    if (_properties.fillDirection.y < 0){
        
        positionCanvas = _viewport_p->viewPortToCanvas( 
            _viewport_p->worldToViewPort( _properties.positionInWorld )
        );

    } else {

        positionCanvas = _viewport_p->viewPortToCanvas( 
            _viewport_p->worldToViewPort(
                {
                    .x = modelBox.x,
                    .y = modelBox.y
                }
            )
        );
    }

    const Vec2D_Int& platformSize = _tileMap.getSizeInTiles();

    for (int _matrix_row = 0; _matrix_row < platformSize.y; _matrix_row++){

        for (int _matrix_col = 0; _matrix_col < platformSize.x; _matrix_col++){

            SpriteAnimationSlice slice = _tileMap.getTileAt(platformSize.y - _matrix_row - 1, _matrix_col);

            _spriteSheet_p->renderSlice(
                {
                    .x = positionCanvas.x + _matrix_col * slice.frame.w,
                    .y = positionCanvas.y - (_matrix_row + 1) * slice.frame.h
                }, 
                slice,
                false
            );
        }
    }
    
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
PlatformModel *RectPlatform::getPhysicsModel_ptr(){
    return &_model;
}

void RectPlatform::_applyOffsetsToFillArea(SDL_Rect &rect){
    // onky downward case for now
    if (_properties.fillDirection.y<0){
        rect.w -= (_properties.offsetMinus + _properties.offsetPlus);
        rect.x += (_properties.offsetMinus);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<SpriteAnimationSlice> RectPlatform::_calculateTileSetFromConfig(){
    
    assert( 
        _properties.tileMapSpriteSliceMatrix.size() == _properties.sliceRotations_in90Deg.size() 
        && _properties.sliceRotations_in90Deg.size() == _properties.sliceFlip_H.size()
        && _properties.sliceFlip_H.size() == (_properties.sizeInTiles.x * _properties.sizeInTiles.y)
    );

    std::vector<SpriteAnimationSlice> slices;

    for (int i = 0; i < (_properties.sizeInTiles.x * _properties.sizeInTiles.y); i++){

        SDL_RendererFlip i_flip = _properties.sliceFlip_V[i] ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;

        if (_properties.sliceFlip_H[i]){
            i_flip = SDL_FLIP_HORIZONTAL;
        }

        // TODO - rotation stuff
        SpriteAnimationSlice i_slice = {
            .duration_ms = 0,
            .quarter_turns_ccw = (short)_properties.sliceRotations_in90Deg[i],
            .flip = i_flip,
            .offset_for_rotation = {
                .x = 0, .y = 0
            },
            .center_of_rotation = {
                .x = 0, .y = 0
            },
            .frame = _spriteSheet_p->getFrameAt(_properties.tileMapSpriteSliceMatrix[i]).frame
        };
        
        slices.push_back(i_slice);
    }

    return slices;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Returns bool value used to decide if the Platform should be rendered.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool RectPlatform::_isVisible(){

    // return true;

    const SDL_FRect &modelBox = _model.getBoundingBox();
    // For platforms with filled adjacent area the visibility detection 
    // must include the fill area
    return (modelBox.x + modelBox.w) > _viewport_p->positionInWorld.x
        && modelBox.x < (_viewport_p->positionInWorld.x + _viewport_p->sizeInWorld.x)
        && (modelBox.y + modelBox.h) > _viewport_p->positionInWorld.y
        && modelBox.y < (_viewport_p->positionInWorld.y + _viewport_p->sizeInWorld.y);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
SDL_FRect RectPlatform::_calculate_WorldTotalArea(){

    assert( !_properties.isMovingPlatform );

    const SDL_FRect actorBox = _model.getBoundingBox();

    // TODO - only fill bellow is considered - include other cases
    if ( _properties.fillDirection.y < 0 ){
        return {
            .x = actorBox.x,
            .y = 0,
            .w = actorBox.w,
            .h = actorBox.y + actorBox.h
        };

    } else {
        throw std::runtime_error("Only 'Fill Bellow' is implented...");
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
SDL_FRect RectPlatform::_calculate_RenderedFillArea(){

    // // Moving Platforms cannot have the "Fill" property set.
    assert(!_properties.isMovingPlatform);

    if (_properties.fillDirection.x > 0){
        

        if (_properties.fillDirection.y < 0){

            // // Fill down
            // if ( 
            //     _model.box.y > _viewport_p->positionInWorld.y
            //     && _model.box.y < _viewport_p->positionInWorld.y + _viewport_p->sizeInWorld.y
            // ){
            //     return {
            //         .w = _model.box.w,
            //         .h = _model.box.y - _viewport_p->positionInWorld.y,
            //         .x = _model.box.x,
            //         .y = _viewport_p->positionInWorld.y
            //     };
            // }

            // naive approach - just pass on the full rectangle

        }
    }

    return {0,0,0,0};
}




///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
bool MovingRectPlatformKeyFrame::compare( const MovingRectPlatformKeyFrame &o1,  const MovingRectPlatformKeyFrame &o2){
    return o1.t_arrival_s < o2.t_arrival_s;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
MovingRectPlatform::MovingRectPlatform(    
    RenderingContext* _cntxt, 
    SpriteSheet* _ss, 
    ViewPort* vp,
    PlatformConfig &p)
:RectPlatform(_cntxt, _ss, vp, p ){

    assert(p.keyPositions_vec.size() > 0);

    _keyFrames = _getKeyFramesFromConfig(p);

    // max_element returns an iterator
    // dereference it to use it.
    // MovingRectPlatformKeyFrame lastEl = *std::max_element( _keyFrames.begin(), _keyFrames.end(), MovingRectPlatformKeyFrame::compare );

    // sort instead
    std::sort( _keyFrames.begin(), _keyFrames.end(), MovingRectPlatformKeyFrame::compare );
    auto lastEl = *_keyFrames.end();

    _totaCycleDuration_s = lastEl.t_arrival_s + lastEl.t_wait_s;
    _currentCycleTime_s = 0;
    _currentKeyFrame_Index = 0;

    const SDL_FRect& oldBB = _model.getBoundingBox();

    SDL_FRect newBB = {
        .x = _keyFrames[ _currentKeyFrame_Index ].p.x,
        .y = _keyFrames[ _currentKeyFrame_Index ].p.y,
        .w = oldBB.w,
        .h = oldBB.h
    };
    
    _model.setBoundingBox( newBB );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void MovingRectPlatform::update(float dt_s){

    const SDL_FRect &modelBox = _model.getBoundingBox();
    const Vec2D_Float &modelVelocity = _model.getVelocity();

    _currentCycleTime_s += dt_s;


    MovingRectPlatformKeyFrame& currentFrame = _keyFrames[_currentKeyFrame_Index];
    MovingRectPlatformKeyFrame& next = _currentKeyFrame_Index < _keyFrames.size() - 1 ?
        _keyFrames[ _currentKeyFrame_Index + 1 ] 
        : _keyFrames[0];
    
    // assume 1 sec of travel time for the last frame.
    
    // current keyFrame is ended - just go to the next
    if (_currentCycleTime_s >= next.t_arrival_s && next.t_arrival_s != 0 ){
        
        _currentKeyFrame_Index++;
        
        _model.setBoundingBox({
            .w = modelBox.w,
            .h = modelBox.h,
            .x = _keyFrames[ _currentKeyFrame_Index ].p.x,
            .y = _keyFrames[ _currentKeyFrame_Index ].p.y
        });

        // _model.velocity.x = 0;
        // _model.velocity.y = 0;
        _model.setVelocity({0,0});

    } else 
    // cycle is turned.
    if ( next.t_arrival_s == 0 && _currentCycleTime_s >=  currentFrame.t_arrival_s + currentFrame.t_wait_s + 2 ){

        _currentCycleTime_s = 0;
        _currentKeyFrame_Index = 0;

        _model.setBoundingBox({
            .w = modelBox.w,
            .h = modelBox.h,
            .x = _keyFrames[ _currentKeyFrame_Index ].p.x,
            .y = _keyFrames[ _currentKeyFrame_Index ].p.y
        });
        _model.setVelocity({0,0});

    } else 
    // current waiting phase is over    
    if (_currentCycleTime_s >= (currentFrame.t_arrival_s + currentFrame.t_wait_s)){
        
        // traveling to next position
        float totalTravelTime = next.t_arrival_s == 0 ?
            2 :
            next.t_arrival_s - (currentFrame.t_arrival_s + currentFrame.t_wait_s);

        float currentTravelTime = _currentCycleTime_s - (currentFrame.t_arrival_s + currentFrame.t_wait_s);

        Vec2D_Float slope = {
            .x = ( next.p.x - currentFrame.p.x ) / totalTravelTime,
            .y = ( next.p.y - currentFrame.p.y ) / totalTravelTime
        };

        _model.setBoundingBox({
            .w = modelBox.w,
            .h = modelBox.h,
            .x = currentFrame.p.x + currentTravelTime * slope.x,
            .y = currentFrame.p.y + currentTravelTime * slope.y
        });
        _model.setVelocity({slope.x,slope.y});

    }

    // else we dont change the position
    _model.update(dt_s);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<MovingRectPlatformKeyFrame> MovingRectPlatform::_getKeyFramesFromConfig(PlatformConfig &properties){
    
    std::vector<MovingRectPlatformKeyFrame> retVal;

    for (auto keyFrameIterator : properties.keyPositions_vec){
        retVal.push_back({
            .p = keyFrameIterator.point,
            .t_arrival_s = keyFrameIterator.timeArrival_s,
            .t_wait_s = keyFrameIterator.timeRest_s
        });
    }

    return retVal;
}

