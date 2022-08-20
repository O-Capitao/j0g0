#include "platform.hpp"
#include "game-defines.hpp"

using namespace j0g0;


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
    PlatformProperties &p)
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


    _model.box.x = _keyFrames[ _currentKeyFrame_Index ].p.x;
    _model.box.y = _keyFrames[ _currentKeyFrame_Index ].p.y;
    

}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void MovingRectPlatform::update(float dt_s){

    _currentCycleTime_s += dt_s;


    MovingRectPlatformKeyFrame& currentFrame = _keyFrames[_currentKeyFrame_Index];
    MovingRectPlatformKeyFrame& next = _currentKeyFrame_Index < _keyFrames.size() - 1 ?
        _keyFrames[ _currentKeyFrame_Index + 1 ] 
        : _keyFrames[0];
    
    // assume 1 sec of travel time for the last frame.
    
    // current keyFrame is ended - just go to the next
    if (_currentCycleTime_s >= next.t_arrival_s && next.t_arrival_s != 0 ){
        
        _currentKeyFrame_Index++;
        
        _model.box.x = _keyFrames[ _currentKeyFrame_Index ].p.x;
        _model.box.y = _keyFrames[ _currentKeyFrame_Index ].p.y;

        _model.velocity.x = 0;
        _model.velocity.y = 0;

    } else 
    // cycle is turned.
    if ( next.t_arrival_s == 0 && _currentCycleTime_s >=  currentFrame.t_arrival_s + currentFrame.t_wait_s + 2 ){

        _currentCycleTime_s = 0;
        _currentKeyFrame_Index = 0;

        _model.box.x = _keyFrames[ _currentKeyFrame_Index ].p.x;
        _model.box.y = _keyFrames[ _currentKeyFrame_Index ].p.y;

        _model.velocity.x = 0;
        _model.velocity.y = 0;

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

        _model.box.x = currentFrame.p.x + currentTravelTime * slope.x;
        _model.box.y = currentFrame.p.y + currentTravelTime * slope.y;

        _model.velocity.x = slope.x;
        _model.velocity.y = slope.y;
    }

    // else we dont change the position
    _model.update(dt_s);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<MovingRectPlatformKeyFrame> MovingRectPlatform::_getKeyFramesFromConfig(PlatformProperties &properties){
    
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
RectPlatform::RectPlatform( 
    RenderingContext* _cntxt, 
    SpriteSheet* _ss, 
    ViewPort* vp,
    PlatformProperties &p
)
:GameObj(_cntxt),
_properties(p),
_model({
        .x = _properties.positionInWorld.x,
        .y = _properties.positionInWorld.y,
        .w = (float)(_properties.sizeInTiles.x * _ss->getSliceSize().x) / vp->pixel_per_meter,
        .h = (float)(_properties.sizeInTiles.y * _ss->getSliceSize().y) / vp->pixel_per_meter
    },
    _properties.ellasticCoef,
    _properties.frictionCoef,
    p.id
),
_spriteSheet_p(_ss),
_tileMap( _ss->getSliceSize(), p.sizeInTiles, _calculateTileSetFromConfig(), p.tileMapSpriteSliceMatrix ),
_viewport_p(vp)
{

    // If a fill direction is specified in the config.
    if (_properties.fillDirection.x != 0 || _properties.fillDirection.y!=0){
        _fillArea = _calculateFillArea();
    }


    #if DEBUG
    printf("Build RectPlatform: %s\n", _properties.id.c_str());
    #endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
RectPlatform::~RectPlatform(){
    _spriteSheet_p = nullptr;
    _viewport_p = nullptr;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void RectPlatform::render(){

    if (!_isVisible()){
        return;
    }

    Vec2D_Int positionCanvas = _viewport_p->viewPortToCanvas( 
        _viewport_p->worldToViewPort(
            {
                .x = _model.box.x,
                .y = _model.box.y
            }
        )
    );

    const Vec2D_Int& platformSize = _tileMap.getSizeInTiles();

    for (int _matrix_row = 0; _matrix_row < platformSize.y; _matrix_row++){

        for (int _matrix_col = 0; _matrix_col < platformSize.x; _matrix_col++){

            SpriteSlice slice = _tileMap.getTileAt(platformSize.y - _matrix_row - 1, _matrix_col);

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

    if (_fillArea.w != 0 || _fillArea.h != 0){
        
        SDL_Rect fill_rect = _viewport_p->transformRect_Viewport2Canvas(
            _viewport_p->transformRect_World2Viewport(
                _calculateFillArea()
            )
        );
        
        SDL_SetRenderDrawColor(context->renderer, 
            _properties.fillColor.r, 
            _properties.fillColor.g, 
            _properties.fillColor.b, 
            _properties.fillColor.a 
        );

        SDL_RenderFillRect(context->renderer, &fill_rect );
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
RectPlatformPhysicsModel *RectPlatform::getPhysicsModel_ptr(){
    return &_model;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<SpriteSlice> RectPlatform::_calculateTileSetFromConfig(){
    
    assert( 
        _properties.tileMapSpriteSliceMatrix.size() == _properties.sliceRotations_in90Deg.size() 
        && _properties.sliceRotations_in90Deg.size() == _properties.sliceFlip_H.size()
        && _properties.sliceFlip_H.size() == (_properties.sizeInTiles.x * _properties.sizeInTiles.y)
    );

    std::vector<SpriteSlice> slices;

    for (int i = 0; i < (_properties.sizeInTiles.x * _properties.sizeInTiles.y); i++){

        SDL_RendererFlip i_flip = _properties.sliceFlip_V[i] ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;

        if (_properties.sliceFlip_H[i]){
            i_flip = SDL_FLIP_HORIZONTAL;
        }

        // TODO - rotation stuff
        SpriteSlice i_slice = {
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
    return (_model.box.x + _model.box.w) > _viewport_p->positionInWorld.x
        && _model.box.x < (_viewport_p->positionInWorld.x + _viewport_p->sizeInWorld.x)
        && (_model.box.y + _model.box.h) > _viewport_p->positionInWorld.y
        && _model.box.y < (_viewport_p->positionInWorld.y + _viewport_p->sizeInWorld.y);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
FloatRect RectPlatform::_calculateFillArea(){
    
    if (_properties.fillDirection.x > 0){
        
        // fill to the Right.

        // Check if there is space between right edge of Viewport
        // and right edge of Platform's BB
        float rightEdgeOfVP_x = _viewport_p->positionInWorld.x + _viewport_p->sizeInWorld.x;
        float rightEdgeOfPlaform_x = _model.box.x + _model.box.w;

        if (
            rightEdgeOfVP_x > rightEdgeOfPlaform_x 
            && rightEdgeOfPlaform_x > _viewport_p->positionInWorld.x
        ){
            return {
                .w = rightEdgeOfVP_x - rightEdgeOfPlaform_x,
                .h = _model.box.h,
                .x = rightEdgeOfPlaform_x,
                .y = _model.box.y
            };
        }

    } else if (_properties.fillDirection.x < 0){
        // Fill to the Left
        if (
            _model.box.x > _viewport_p->positionInWorld.x 
            && _model.box.x < _viewport_p->positionInWorld.x + _viewport_p->sizeInWorld.x
        ){
            return {
                .w = _model.box.x - _viewport_p->positionInWorld.x,
                .h= _model.box.h,
                .x = 0,
                .y = _model.box.y
            };
        }


    } else if (_properties.fillDirection.y > 0){
        // Fill up
        float topEdgeVP_y = _viewport_p->positionInWorld.y + _viewport_p->sizeInWorld.y;
        float topEdgeOfPlatform_y = _model.box.y + _model.box.h;

        if (
            topEdgeOfPlatform_y < topEdgeVP_y
            && topEdgeOfPlatform_y > _viewport_p->positionInWorld.y
        ){
            return {
                .w = _model.box.w,
                .h = topEdgeVP_y - topEdgeOfPlatform_y,
                .x = _model.box.x,
                .y = topEdgeOfPlatform_y
            };
        }

    }else if (_properties.fillDirection.y < 0){
        // Fill down
        if ( 
            _model.box.y > _viewport_p->positionInWorld.y
            && _model.box.y < _viewport_p->positionInWorld.y + _viewport_p->sizeInWorld.y
        ){
            return {
                .w = _model.box.w,
                .h = _model.box.y - _viewport_p->positionInWorld.y,
                .x = _model.box.x,
                .y = 0
            };
        }

    }

    return {0,0,0,0};
}
