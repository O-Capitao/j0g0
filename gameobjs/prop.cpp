#include "prop.hpp"
#include "game-defines.hpp"

using namespace j0g0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
Prop::Prop(
    RenderingContext* c, 
    SpriteSheet* ss,
    ViewPort *vp,
    PropConfig &p
)
:GameObj(c),
_config(p),
_box({
    .x = p.positionInWorld.x,
    .y = p.positionInWorld.y,
    .w = vp->scaleToWorld((float)ss->getSliceSize().x),
    .h = vp->scaleToWorld((float)ss->getSliceSize().y)
}),
_spriteSheet_p(ss),
_viewPort_p(vp)
{
#if DEBUG
    printf("Constructed Prop for %s.\n", p.id.c_str());
#endif
}

void Prop::render(){

    if (_isInViewPort() ){
        SDL_Rect _boxInCanvas = _viewPort_p->transformRect_Viewport2Canvas(
            _viewPort_p->transformRect_World2Viewport( _box )
        );

        _spriteSheet_p->renderBySliceIndex({
            .x = _boxInCanvas.x, 
            .y = _boxInCanvas.y
        }, _config.spriteSliceIndex);
    }
    
}

bool Prop::_isInViewPort(){
    return _box.overlapsWith({
        .x= _viewPort_p->positionInWorld.x,
        .y= _viewPort_p->positionInWorld.y,
        .w = _viewPort_p->sizeInWorld.x,
        .h = _viewPort_p->sizeInWorld.y
    });
}