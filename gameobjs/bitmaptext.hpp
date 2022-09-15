#pragma once

#include "gameobj.hpp"
#include "graphics/spritesheet.hpp"

namespace j0g0 {
    struct BitmapTextLine : public GameObj {

        BitmapTextLine(
            RenderingContext* _context, SpriteSheet* _spriteSheet
        );

        void render();
        void update(float dt){};

        void setContent(const std::string &message);
        void setPositionInCanvas(const Vec2D_Int &_newPosition);
        const Vec2D_Int &getPositionInCanvas(){ return _positionInCanvas; };
        const Vec2D_Int &getContentSize();
        
        private:
            SpriteSheet* _spriteSheet_p;
            const std::string _characterMap = "ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890?!,.:";
            
            std::string _content;
            std::vector<SpriteAnimationSlice> _slices;
            Vec2D_Int _positionInCanvas;
            Vec2D_Int _charSize;

            Vec2D_Int _contentSize_InCanvas;
            
    };
}