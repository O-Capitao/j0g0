#include <iostream>

#include "SDL_image.h"

#include "spritesheet.hpp"

using namespace j0g0;

SpriteSheet::SpriteSheet(RenderingContext* _context)
    : context(_context)
{   
    texture = NULL;

	width = 0;
	height = 0;
    n_rows = 0;
    n_columns = 0;
    step_x = 0;
    step_y = 0;

    ori_path = "unassigned";
}

SpriteSheet::~SpriteSheet()
{
	_free();
}

void SpriteSheet::initSpriteSheet(std::string path,int _x_divisions,  int _y_divisions, int _scaling_factor)
{
    ori_path = path;

    _loadFromFile(path);

    int _counter = 0;

    scaling_factor = _scaling_factor;
    step_x = width / _x_divisions;
    step_y = height / _y_divisions;

    n_columns = _x_divisions;
    n_rows = _y_divisions;

    for (int i = 0; i < _y_divisions; i++){
        for (int j = 0; j < _x_divisions; j++){

            SpriteFrame _frame;

            _frame.index = _counter;

            _frame.frame = {
                .w = step_x,
                .h = step_y,
                .x = j * step_x,
                .y = i * step_y
            };

            _frame.frame.w = step_x;
            _frame.frame.h = step_y;
            
            _frame.frame.x = j * step_x;
            _frame.frame.y = i * step_y;

            frames.push_back(_frame);

            _counter++;
        }
    }
    
}

bool SpriteSheet::_loadFromFile( std::string& path )
{
    //Get rid of preexisting texture
	_free();

    //The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( context->renderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			width = loadedSurface->w;
			height = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	texture = newTexture;

    return texture != NULL;
}

void SpriteSheet::_free()
{
    //Free texture if it exists
	if( texture != NULL )
	{
		SDL_DestroyTexture( texture );
		texture = NULL;
		width = 0;
		height = 0;
	}
}

void SpriteSheet::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( texture, red, green, blue );
}

void SpriteSheet::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( texture, blending );
}
		
void SpriteSheet::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( texture, alpha );
}

void SpriteSheet::render( int x, int y, int frame_index )
{
    SDL_Rect rect = frames[frame_index].frame;
    SDL_Rect tgt;
    tgt.x = x;
    tgt.y = y;
    tgt.w = scaling_factor * step_x;
    tgt.h = scaling_factor * step_y;
    
	SDL_RenderCopy( context->renderer, texture, &rect, &tgt );
}

SpriteFrame SpriteSheet::getFrameAt(int index){
    return frames[index];
}
