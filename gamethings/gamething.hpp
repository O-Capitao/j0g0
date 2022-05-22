#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "SDL.h"

#include "graphics/gamegeometry.hpp"
#include "graphics/spritesheet.hpp"
#include "graphics/viewport.hpp"

namespace j0g0 {





    /*
     *  Base Class for all Stuff on Screen
     * GameThing is not aware of World Space -> all dimesions and props are defined 
     * in canvas space
     */
    struct GameThing {

        GameThing(RenderingContext* _context, const SDL_Rect& _positionInCanvas );
        virtual ~GameThing() = 0;

        virtual void render() = 0;
        virtual void update() = 0;

        void setBoundingBoxPositionInCanvas( const Vec2D_Int& _position );
        void centerOnCanvas();

        protected:
        
            RenderingContext* context;
            std::string id;

            SDL_Rect boundingBoxInCanvas;
            
    };





    // /*
    //  * "Pure" drawing -> no media resources
    //  */
    // struct GeometricalThing: public GameThing {

    //     std::vector<Vec2D_Float> points;
    //     static int counter;

    //     GeometricalThing(RenderingContext* _context, const SDL_Rect& _positionInCanvas);

    //     ~GeometricalThing();
        
    //     void renderToViewPort( ViewPort& viewport);
    //     void renderToCanvas();
    //     void update();
    // };





    /*
     * SpriteThing ->
     *  - Contains one sprite sheet
     *  - Contains multiple animations (sequences) using frames
     *    from the same Sprite Sheet
     */
    struct SpriteThing: public GameThing {

        SpriteThing( 
            RenderingContext* _context,
            std::string path_to_img,
            int n_frames,
            int x_divisions,
            int y_divisions,
            const Vec2D_Float& initialPosition,
            ViewPort* _viewport
        );

        ~SpriteThing();

        void render();
        void update();

        protected:

            FloatRect boundingBoxInWorld;
            SpriteSheet spriteSheet;
            std::unordered_map<int, SpriteSheetAnimation> animations;
            SpriteSheetAnimation active_animation;
            int last_tick;
            bool moveForward = true;

            void setActiveAnimation( int animation_id );
            void initAnimation( int animation_id, int start_index, int n_frames, int* frame_durations );
            void setMovementDirection(bool _moveForward);

        private:

            int frame_counter;
            int time_since_frame_changed;
            ViewPort* viewPort_p = NULL;

            void _calculate_CanvasPosition();

    };





    // Uses Sprite Sheet with all usable characters
    // and maps each character in a string
    // into a drawable texture
    struct BitmapTextThing: public GameThing {

        BitmapTextThing(
            RenderingContext* _context, int _scaling
        );


        ~BitmapTextThing();

        void render();
        void update(){};

        void setContent(const std::string& message);

        private:
            SpriteSheet sprite_sheet;
            const std::string character_map = "ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890?!,.:";
            SDL_Texture* composite_texture;
            int scaling;

            void _initCompositeTexture();
            
    };
}
