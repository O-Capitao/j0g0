
#include "bitmaptext.hpp"


using namespace j0g0;

/*
* Text Sprite Thing
*
*/
BitmapText::BitmapText(RenderingContext* _context,  SpriteSheet* _spriteSheet )
:GameObj(_context )
{
    _spriteSheet_p = _spriteSheet;
    _charSize = _spriteSheet_p->getSliceSize();
}

BitmapText::~BitmapText(){
        
}

void BitmapText:: render()
{   
    int index = 0;
    for (const SpriteAnimationSlice &_slice_ref : _slices ){
        _spriteSheet_p->renderSlice_Simple( {
                .x = _positionInCanvas.x + (index++) * _charSize.x,
                .y = _positionInCanvas.y
            }
            ,_slice_ref 
        );
    }
}

void BitmapText::setContent(const std::string& input){

    int input_length = input.length();

    _contentSize_InCanvas = {
        .x = input_length * _charSize.x,
        .y = _charSize.y
    };

    for (int i = 0; i < input.length(); i++){

        char current_char = input[i];
        int _char_index = _characterMap.find(current_char);

        _slices.push_back( _spriteSheet_p->getFrameAt( _char_index ) );
    }
}

void BitmapText::setPositionInCanvas(const Vec2D_Int &_newPosition){
    _positionInCanvas.x = _newPosition.x;
    _positionInCanvas.y = _newPosition.y;
}

const Vec2D_Int &BitmapText::getContentSize(){
    return _contentSize_InCanvas;
}




