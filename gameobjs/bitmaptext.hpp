#pragma once

#include "gameobj.hpp"
#include "graphics/spritesheet.hpp"

namespace j0g0 {
    struct BitmapText : public GameObj {

        BitmapText(
            RenderingContext* _context, SpriteSheet* _spriteSheet
        );

        ~BitmapText();

        void render();
        void update(){};

        void setContent(const std::string& message);
        void setPositionInCanvas(const Vec2D_Int &_newPosition);
        
        private:
            SpriteSheet* _spriteSheet_p;
            const std::string _characterMap = "ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890?!,.:";
            
            SDL_Texture* _compositeTexture_p;
            Vec2D_Int _positionInCanvas;
            Vec2D_Int _compositeTextureSize;
            Vec2D_Int _charSize;

            void _initCompositeTexture();
            
    };
}