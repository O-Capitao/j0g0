#include <iostream>
#include <math.h>  
#include <exception>

#include "SDL_image.h"

#include "rendering.hpp"

using namespace j0g0;



RenderingContext::~RenderingContext(){
    
    //Destroy window	
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
    SDL_DestroyTexture(canvas);
    
	window = NULL;
	renderer = NULL;
    canvas = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

void RenderingContext::init(int windowWidth, int windowHeight, int _pixelRatio ){
    
    // assert that we are dealing with pixel-perfect dimensions.
    assert( (windowWidth%_pixelRatio == 0) && (windowWidth%_pixelRatio == 0) );
    
    canvasProperties.pixelRatio = 4;
    canvasProperties.size.x = windowWidth / 4;
    canvasProperties.size.y = windowHeight / 4;

    int sdlIsInitted = SDL_Init( SDL_INIT_VIDEO );

    assert(sdlIsInitted >= 0);

    window = SDL_CreateWindow( 
        "SDL Tutorial", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        windowWidth, 
        windowHeight, 
        SDL_WINDOW_SHOWN
    );
    assert( window != NULL );

    // Create vsynced renderer for window
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    assert( renderer != NULL ); 

    SDL_RendererInfo info;

    /* Checking if this renderer supports target textures */
    SDL_GetRendererInfo(renderer, &info);
    int renderer_has_target_texture_support = info.flags & SDL_RENDERER_TARGETTEXTURE;

    SDL_Log("Renderer %s started.", info.name);

    assert(renderer_has_target_texture_support);

    SDL_SetRenderDrawBlendMode( renderer, SDL_BLENDMODE_BLEND );

    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    int initPngSupport = IMG_Init( imgFlags );

    assert(initPngSupport >= 0);

    // setup Canvas texture
    canvas = SDL_CreateTexture(
        renderer, 
        SDL_PIXELFORMAT_ARGB8888, 
        SDL_TEXTUREACCESS_TARGET, 
        canvasProperties.size.x, 
        canvasProperties.size.y
    );
	
    assert(canvas != NULL );
}


void RenderingContext::beginRenderStep(){

    // allow transparent pixels
    SDL_SetTextureBlendMode(canvas, SDL_BLENDMODE_BLEND);

    // set this texture as the target for following
    // Render calls
    SDL_SetRenderTarget( renderer, canvas );

    /* It's always a good idea to clear the whole thing first. */
    SDL_SetRenderDrawColor( renderer, 
        canvasProperties.backgroundColor.r, 
        canvasProperties.backgroundColor.g,
        canvasProperties.backgroundColor.b, 
        canvasProperties.backgroundColor.a );

    SDL_RenderClear( renderer );
}

void RenderingContext::endRenderStep(){

    SDL_SetRenderTarget( renderer, NULL);
    
    // upscale the canvas
    SDL_Rect src, tgt;
    src.x = 0;
    src.y = 0;
    src.w = canvasProperties.size.x;
    src.h = canvasProperties.size.y;

    tgt.y = 0;
    tgt.x = 0;
    tgt.w = canvasProperties.pixelRatio * canvasProperties.size.x;
    tgt.h = canvasProperties.pixelRatio * canvasProperties.size.y;

    SDL_RenderCopy(renderer, canvas, &src, &tgt);
    
    SDL_RenderPresent(renderer);

}

void RenderingContext::setBackgroundColor(const SDL_Color& _color){
    canvasProperties.backgroundColor.r = _color.r;
    canvasProperties.backgroundColor.g = _color.g;
    canvasProperties.backgroundColor.b = _color.b;
    canvasProperties.backgroundColor.a = _color.a;
}