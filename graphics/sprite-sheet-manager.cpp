#include "spritesheet.hpp"
#include "assert.h"
#include "game-defines.hpp"

using namespace j0g0;


SpriteSheetManager::SpriteSheetManager(){
    #if DEBUG
    printf("Constructing : SpriteSheetManager::SpriteSheetManager()\n");
    #endif
}

SpriteSheetManager::~SpriteSheetManager(){
    
    #if DEBUG
    printf("Destroying : SpriteSheetManager::~SpriteSheetManager()\n");
    #endif
    
    for (auto sh: available_sheets){
        delete sh.second;
    }

    available_sheets.clear();
}

SpriteSheet* SpriteSheetManager::getSpriteSheet( const std::string &spriteKey ){
    auto map_node = available_sheets.find(spriteKey);
    return map_node->second;
}

SpriteSheet* SpriteSheetManager::insertSpriteSheet( 
    std::string spriteKey, 
    RenderingContext* _context,
    const std::string &path, 
    const Vec2D_Int &sliceSize, 
    int _scaling_factor
){ 
    // assert( available_sheets.find(spriteKey) == available_sheets.end());

    auto coiso = new SpriteSheet(_context,path,sliceSize,_scaling_factor);

    available_sheets[spriteKey] = coiso;

    return available_sheets[spriteKey];
}

void SpriteSheetManager::removeAndDestroySpriteSheet( const std::string &spriteKey ){

}

