#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "SDL.h"

#include "rendering.hpp"
#include "gamegeometry.hpp"

namespace j0g0
{

    struct SpriteSlice {
        
        float duration_ms;
        short quarter_turns_ccw = 0;
        
        SDL_RendererFlip flip;
        Vec2D_Int offset_for_rotation;
        SDL_Point center_of_rotation;
        SDL_Rect frame;
    };

    struct SpriteAnimation {
        std::string id;
        std::vector<SpriteSlice> animation_frames;
    };

    /*
    * Wraps around SDL_Texture.
    * Contains array of SpriteFrames -> containing all Frames in the sheet.
    */
    struct SpriteSheet
    {
        SpriteSheet( 
            RenderingContext* _context,
            const std::string &path, 
            const Vec2D_Int &sliceSize, 
            int _scaling_factor = 1 
        );

        ~SpriteSheet();

        // Set color modulation
        void setColor( Uint8 red, Uint8 green, Uint8 blue );

        // Set blending
        void setBlendMode( SDL_BlendMode blending );

        // Set alpha modulation
        void setAlpha( Uint8 alpha );

		// Renders texture at given point
		void renderBySliceIndex( const Vec2D_Int &positionInCanvas, int frame_index );

        // if overrideFlip is true -> flip will be undone if specified in Sprite Frame, 
        // or added if false in the Sprite Frame
        void renderSlice( const Vec2D_Int &positionInCanvas, const SpriteSlice &slice, bool overrideHorizontalFlip);

        void renderSlice_Simple( const Vec2D_Int &positionInCanvas, const SpriteSlice &slice);

        SpriteSlice getFrameAt(int index);

        SpriteAnimation createAnimation( std::vector<int> slice_ids);

        const Vec2D_Int& getSliceSize();

        private:

            Vec2D_Int _size;
            Vec2D_Int _sliceSize;

            int _n_rows, _n_columns;
            int _scalingFactor;
            // int step_x, step_y;

            std::string _ori_path;
            std::vector<SpriteSlice> _slices; // all slices from image -> no rotation or flip
            
            SDL_Texture* _texture_p;
            RenderingContext* _context_p;

            // Deallocates texture
            void _free();

            // Loads image at specified path
            bool _loadFromFile( std::string& path );

            void _initSpriteSheet();

            
    };

    struct SpriteSheetManager {

        SpriteSheetManager();

        ~SpriteSheetManager();

        SpriteSheet* getSpriteSheet( const std::string &spriteKey );
        
        SpriteSheet* insertSpriteSheet( 
            std::string spriteKey, 
            RenderingContext* _context,
            const std::string &path, 
            const Vec2D_Int &sliceSize, 
            int _scaling_factor
        );

        void removeAndDestroySpriteSheet( const std::string &spriteKey );

        private:
            std::unordered_map<std::string,SpriteSheet*> available_sheets;
    };
}

