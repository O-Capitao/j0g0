#include <iostream>

#include "gamelevel.hpp"
#include "gamethings/tilemapthing.hpp"
#include "gamethings/playerthing.hpp"

using namespace j0g0;

GameLevel::GameLevel( const GameLevelProperties& props, RenderingContext* _context )
: viewport( ViewPort( {.x=0,.y=0}, 10, _context->canvasProperties ) ) {

    worldSize = props.size;
    backgroundColor = {.r = 255, .g = 255, .b= 235, .a = 255 };
    
    // // init terrain
    // TileMapThing* thing = new TileMapThing(_context, "asset/platformThing.png", 9, 3, 3, &viewport);

    // Vec2D_Float blockP = { .x = 0, .y = 0.5 };
    // Vec2D_Int blockSize = {.x = 20, .y = 1 };

    // int blockIndex = thing->createBlock(
    //     blockP,
    //     blockSize
    // );

    // TileBlock* ground = thing->tileBlocks[blockIndex];
    // ground->setAllTiles(1);

    // things.push_back(thing);

    PlayerThing* mything = new PlayerThing(  
        _context, 
        "asset/crocboy.png", 
        20, 
        12, 
        2, 
        {.x=1.0f,.y=4.0f},
        &viewport
    );
    player_thing_p = mything;
    things.push_back( mything );
}

GameLevel::~GameLevel(){

    for (auto p : things)
    {
        delete p;
        p = NULL;
    } 

    things.clear();
}

void GameLevel::render(){
    
    for (int i = 0; i < things.size(); i++){
        
        things[i]->render();

    }

}

void GameLevel::update(){
    for (int i = 0; i < things.size(); i++){

        things[i]->update();

    }
}

void GameLevel::handleEvents( const SDL_Event& evt ){
    player_thing_p->handleEvent( evt );
}