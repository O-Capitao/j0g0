#include <iostream>

#include "SDL_image.h"

#include "spritesheet.hpp"

using namespace j0g0;

SpriteSheet::SpriteSheet(
    RenderingContext* _context,
    const std::string &path, 
    const Vec2D_Int &sliceSize, 
    int _scaling_factor)
:_context_p(_context) {

    _texture_p = NULL;
    _sliceSize = {
        .x = sliceSize.x, .y= sliceSize.y
    };

    _ori_path = path;
    _scalingFactor = _scaling_factor;

	_initSpriteSheet();
}

SpriteSheet::~SpriteSheet()
{
	_free();
}

void SpriteSheet::_initSpriteSheet()
{
    _loadFromFile(_ori_path);

    int _counter = 0;

    _n_columns = _size.x / _sliceSize.x;
    _n_rows = _size.y / _sliceSize.y;

    for (int i = 0; i < _n_rows; i++){
        for (int j = 0; j < _n_columns; j++){

            SpriteAnimationSlice _frame;

            // _frame.index = _counter;

            _frame.frame = {
                .w = _sliceSize.x,
                .h = _sliceSize.y,
                .x = j * _sliceSize.x,
                .y = i * _sliceSize.y
            };

            _frame.center_of_rotation = {
                .x = _frame.frame.x,
                .y = _frame.frame.y
            };

            _slices.push_back(_frame);

            _counter++;
        }
    }
    
}

bool SpriteSheet::_loadFromFile( std::string& path )
{
    //Get rid of preexisting _texture_p
	_free();

    //The final _texture_p
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Create _texture_p from surface pixels
        newTexture = SDL_CreateTextureFromSurface( _context_p->renderer, loadedSurface );
        
		if( newTexture == NULL )
		{
			printf( "Unable to create _texture_p from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
            _size = {
                .x = loadedSurface->w,
                .y = loadedSurface->h
            };
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	_texture_p = newTexture;

    return _texture_p != NULL;
}

void SpriteSheet::_free()
{
    //Free _texture_p if it exists
	if( _texture_p != NULL )
	{
		SDL_DestroyTexture( _texture_p );
		_texture_p = NULL;
	}
}

void SpriteSheet::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate _texture_p rgb
	SDL_SetTextureColorMod( _texture_p, red, green, blue );
}

void SpriteSheet::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( _texture_p, blending );
}
		
void SpriteSheet::setAlpha( Uint8 alpha )
{
	//Modulate _texture_p alpha
	SDL_SetTextureAlphaMod( _texture_p, alpha );
}

void SpriteSheet::renderBySliceIndex( const Vec2D_Int &positionInCanvas, int frame_index )
{

    SpriteAnimationSlice& slice = _slices[frame_index];
    renderSlice(positionInCanvas, slice, false );
}

void SpriteSheet::renderSlice( const Vec2D_Int &positionInCanvas, const SpriteAnimationSlice &slice, bool overrideHorizontalFlip )
{
    SDL_Rect src = slice.frame;
    SDL_Rect tgt;
    
    tgt.w = _scalingFactor * _sliceSize.x;
    tgt.h = _scalingFactor * _sliceSize.y;

    tgt.x = positionInCanvas.x + slice.offset_for_rotation.x;
    tgt.y = positionInCanvas.y + slice.offset_for_rotation.y;

    SDL_RendererFlip effectiveHorizontalFlip = slice.flip;
    if (overrideHorizontalFlip){

        if (effectiveHorizontalFlip == SDL_FLIP_NONE){

            effectiveHorizontalFlip = SDL_FLIP_HORIZONTAL;
        
        } else {
        
            effectiveHorizontalFlip = SDL_FLIP_NONE;
        
        }

    }
    
	SDL_RenderCopyEx( 
        _context_p->renderer, _texture_p, 
        &src, 
        &tgt, 
        slice.quarter_turns_ccw * 90, 
        &slice.center_of_rotation, 
        effectiveHorizontalFlip
    );
}

void SpriteSheet::renderSlice_Simple( const Vec2D_Int &positionInCanvas, const SpriteAnimationSlice &slice )
{
    SDL_Rect src = slice.frame;
    SDL_Rect tgt;

    tgt.x = positionInCanvas.x;
    tgt.y = positionInCanvas.y;
    
    tgt.w = _scalingFactor * _sliceSize.x;
    tgt.h = _scalingFactor * _sliceSize.y;
    
	SDL_RenderCopy( 
        _context_p->renderer, _texture_p, 
        &src, &tgt
    );
}

SpriteAnimationSlice SpriteSheet::getFrameAt(int index){
    return _slices[index];
}

const Vec2D_Int& SpriteSheet::getSliceSize(){
    return _sliceSize;
}
