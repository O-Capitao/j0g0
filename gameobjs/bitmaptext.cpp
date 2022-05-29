
#include "bitmaptext.hpp"


using namespace j0g0;

/*
* Text Sprite Thing
*
*/
BitmapText::BitmapText(RenderingContext* _context,  SpriteSheet* _spriteSheet )
:GameObj(_context ),
_compositeTextureSize({.x=0,.y=0})
{
    _spriteSheet_p = _spriteSheet;
    _charSize = _spriteSheet_p->getSliceSize();
    _compositeTexture_p = NULL;    
}

BitmapText::~BitmapText(){
    
    SDL_DestroyTexture(_compositeTexture_p);
    _compositeTexture_p = NULL;
    
}

void BitmapText::render()
{

    SDL_Rect src = {
        .x = 0,
        .y = 0,
        .w = _compositeTextureSize.x,
        .h = _compositeTextureSize.y
    };


    SDL_Rect tgt = {
        .x = _positionInCanvas.x,
        .y = _positionInCanvas.y,
        .w = _compositeTextureSize.x,
        .h = _compositeTextureSize.y
    };

    SDL_RenderCopy( 
        context->renderer, 
        _compositeTexture_p, 
        &src, 
        &tgt);
}

void BitmapText::_initCompositeTexture(){

    _compositeTexture_p = SDL_CreateTexture(
        context->renderer, 
        SDL_PIXELFORMAT_ARGB8888, 
        SDL_TEXTUREACCESS_TARGET, 
        _compositeTextureSize.x,
        _compositeTextureSize.y
    );

    // allow transparent pixels
    SDL_SetTextureBlendMode(_compositeTexture_p, SDL_BLENDMODE_BLEND);

}

void BitmapText::setContent(const std::string& input){

    Vec2D_Int charSize = _spriteSheet_p->getSliceSize();

    int input_length = input.length();
    
    _compositeTextureSize.x = input_length * _charSize.x;
    _compositeTextureSize.y= _charSize.y;

    if (_compositeTexture_p == NULL){
        _initCompositeTexture();
    }

    // set this texture as the target for following
    // Render calls
    SDL_SetRenderTarget(context->renderer, _compositeTexture_p);
    SDL_SetRenderDrawColor( context->renderer, 0, 0, 0, 0 );
    SDL_RenderClear(context->renderer);

    for (int i = 0; i < input.length(); i++){

        char current_char = input[i];
        int _char_index = _characterMap.find(current_char);

        _spriteSheet_p->render( _positionInCanvas, _char_index );

    }

    /* Resetting to the default render target which is the frame buffer
       that gets displayed on screen. */
    SDL_SetRenderTarget(context->renderer, NULL);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "exiting setContent");
}

void BitmapText::setPositionInCanvas(const Vec2D_Int &_newPosition){
    _positionInCanvas.x = _newPosition.x;
    _positionInCanvas.y = _newPosition.y;
}




