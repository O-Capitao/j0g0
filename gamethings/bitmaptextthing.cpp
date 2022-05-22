
#include "gamething.hpp"


using namespace j0g0;

/*
* Text Sprite Thing
*
*/
BitmapTextThing::BitmapTextThing(RenderingContext* _context, int _scaling )
:GameThing(_context, {.w=0,.h=0,.x=0,.y=0} ),
sprite_sheet(SpriteSheet(_context))
{
    id = "Text.";
    
    sprite_sheet.initSpriteSheet(
        "asset/text2.png",
        26, // x divisions
        2,  // y divisions
        _scaling   // scaling
    );

    scaling = _scaling;
    composite_texture = NULL;
    
}

BitmapTextThing::~BitmapTextThing(){
    
    SDL_DestroyTexture(composite_texture);
    composite_texture = NULL;
    
}

void BitmapTextThing::render()
{

    SDL_Rect src;
    src.x = 0;
    src.y = 0;
    src.w = this->boundingBoxInCanvas.w; // size of compositeTexture
    src.h = this->boundingBoxInCanvas.h;

    SDL_RenderCopy( 
        context->renderer, composite_texture, 
        &src, 
        &(this->boundingBoxInCanvas));
}

void BitmapTextThing::_initCompositeTexture(){

    composite_texture = SDL_CreateTexture(
        context->renderer, 
        SDL_PIXELFORMAT_ARGB8888, 
        SDL_TEXTUREACCESS_TARGET, 
        this->boundingBoxInCanvas.w,
        this->boundingBoxInCanvas.h
    );

    // allow transparent pixels
    SDL_SetTextureBlendMode(composite_texture, SDL_BLENDMODE_BLEND);

}

void BitmapTextThing::setContent(const std::string& input){

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "entering setContent. Message: ");
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, input.c_str()) ;

    int input_length = input.length();
    this->boundingBoxInCanvas.w = input_length * sprite_sheet.step_x;
    // do some kind of chck for line breaks?
    // for now we print 1 line
    this->boundingBoxInCanvas.h = sprite_sheet.step_y;

    if (composite_texture == NULL){
        _initCompositeTexture();
    }

    // set this texture as the target for following
    // Render calls
    SDL_SetRenderTarget(context->renderer, composite_texture);

    /* It's always a good idea to clear the whole thing first. */
    SDL_SetRenderDrawColor( context->renderer, 0, 0, 0, 0 );

    SDL_RenderClear(context->renderer);

    for (int i = 0; i < input.length(); i++){

        char current_char = input[i];
        int found = character_map.find(current_char);

        SDL_Rect src, tgt;
        src.w = sprite_sheet.step_x;
        src.h = sprite_sheet.step_y;
        src.x = found * sprite_sheet.step_x;
        src.y = 0; // just hardcode it for now...

        tgt.w = src.w;
        tgt.h = src.h;

        tgt.x = i * sprite_sheet.step_x;
        tgt.y = 0;

        SDL_RenderCopy(context->renderer, sprite_sheet.texture, &src, &tgt);
    }

    /* Resetting to the default render target which is the frame buffer
       that gets displayed on screen. */
    SDL_SetRenderTarget(context->renderer, NULL);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "exiting setContent");
}





