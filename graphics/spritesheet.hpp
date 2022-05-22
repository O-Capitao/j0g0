#pragma once

#include <string>
#include <vector>

#include "SDL.h"

#include "rendering.hpp"
#include "gamegeometry.hpp"

namespace j0g0
{

    struct SpriteFrame {
        int index;
        float duration_ms;
        SDL_Rect frame;
    };





    struct SpriteSheetAnimation {
        std::string id;
        std::vector<SpriteFrame> animation_frames;
    };







    /*
    * Wraps around SDL_Texture.
    * Contains array of SpriteFrames -> containing all Frames in the sheet.
    */
    struct SpriteSheet
    {
        SpriteSheet(RenderingContext* _context);
        ~SpriteSheet();

        // Set color modulation
        void setColor( Uint8 red, Uint8 green, Uint8 blue );

        // Set blending
        void setBlendMode( SDL_BlendMode blending );

        // Set alpha modulation
        void setAlpha( Uint8 alpha );

		// Renders texture at given point
		void render( int x, int y, int frame_index );

        void initSpriteSheet( std::string path, int _x_divisions = 1, int _y_divisions = 1, int _scaling_factor = 1);
        
        int width, height;    
        int n_rows, n_columns;
        int scaling_factor;
        int step_x, step_y;
        
        SDL_Texture* texture;
        
        SpriteFrame getFrameAt(int index);

        private:

            // Deallocates texture
            void _free();

            // Loads image at specified path
            bool _loadFromFile( std::string& path );

            RenderingContext* context;
  
            std::vector<SpriteFrame> frames;

            std::string ori_path;
    };  


}

