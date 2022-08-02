#include "platform.hpp"

using namespace j0g0;

bool MovingRectPlatformKeyFrame::compare( const MovingRectPlatformKeyFrame &o1,  const MovingRectPlatformKeyFrame &o2){
    return o1.t_arrival_s < o2.t_arrival_s;
}

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

void MovingRectPlatform::update(Uint32 dt){

    float dt_s = ((float)dt) / 1000;

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

        return;

    }

    // cycle is turned.
    if ( next.t_arrival_s == 0 && _currentCycleTime_s >=  currentFrame.t_arrival_s + currentFrame.t_wait_s + 2 ){

        _currentCycleTime_s = 0;
        _currentKeyFrame_Index = 0;

        _model.box.x = _keyFrames[ _currentKeyFrame_Index ].p.x;
        _model.box.y = _keyFrames[ _currentKeyFrame_Index ].p.y;

        return;

    }

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

        return;
    }

    // else we dont change the position
    

}

Vec2D_Float _findPositionAtTime( float t_s );


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





RectPlatform::RectPlatform( 
    RenderingContext* _cntxt, 
    SpriteSheet* _ss, 
    ViewPort* vp,
    PlatformProperties &p
)
:GameObj(_cntxt),
_properties(p),
_spriteSheet_p(_ss),
_model({
        .x = _properties.positionInWorld.x,
        .y = _properties.positionInWorld.y,
        .w = (float)(_properties.sizeInTiles.x * _ss->getSliceSize().x) / vp->pixel_per_meter,
        .h = (float)(_properties.sizeInTiles.y * _ss->getSliceSize().y) / vp->pixel_per_meter
    },
    _properties.ellasticCoef,
    _properties.frictionCoef
),
_tileMap( _ss->getSliceSize(), p.sizeInTiles, _calculateTileSetFromConfig(), p.tileMapSpriteSliceMatrix ),
_viewport_p(vp)
{}

RectPlatform::~RectPlatform(){
    _spriteSheet_p = nullptr;
    _viewport_p = nullptr;
}

void RectPlatform::render(){

    Vec2D_Int positionCanvas = _viewport_p->viewPortToCanvas( 
        _viewport_p->worldToViewPort( //_properties.positionInWorld ) 
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
                slice
            );
        }
    }
}

RectPlatformPhysicsModel *RectPlatform::getPhysicsModel_ptr(){
    return &_model;
}

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

