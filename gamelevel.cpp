#include <iostream>

#include "gamelevel.hpp"

using namespace j0g0;

GameLevel::GameLevel( std::string levelPropsFile, RenderingContext* _context, SpriteSheetManager *ssm)
:_context_p(_context),
_ssm_p(ssm),
_viewport( ViewPort( {.x=0,.y=0}, 10, _context->canvasProperties ) )
{

    _properties = _reader.buildGameLevelProperties( levelPropsFile );
    
    _reader.addLevelSpritesToManager(
        levelPropsFile,
        _context_p,
        _ssm_p
    );



    // do init with Properties
}

GameLevel::~GameLevel(){

    // for (auto p : things)
    // {
    //     delete p;
    //     p = NULL;
    // } 

    // things.clear();
}

void GameLevel::render(){
    
    // for (int i = 0; i < things.size(); i++){
        
    //     things[i]->render();

    // }

}

void GameLevel::update(){
    // for (int i = 0; i < things.size(); i++){

    //     things[i]->update();

    // }
}

void GameLevel::handleEvents( const SDL_Event& evt ){
    // player_thing_p->handleEvent( evt );
}

GameLevelProperties GameLevelConfigReader::buildGameLevelProperties( std::string config_path ){

    YAML::Node levelProperties = YAML::LoadFile( config_path );

    auto title = levelProperties["title"].as<std::string>();
    auto worldSize = _parseToVec2D_Float(levelProperties["world-size"]);
    auto color = _parseToSDL_Color(levelProperties["background-color"]);

    return {
        .title = title,
        .backgroundColor = color,
        .worldSize = worldSize
    };
}   

void GameLevelConfigReader::addLevelSpritesToManager( 
    std::string config_path,
    RenderingContext *context,
    SpriteSheetManager *ssm
){

    YAML::Node levelProperties = YAML::LoadFile( config_path );
    YAML::Node spriteSheets = levelProperties["sprite-sheets"];

    for (std::size_t i=0 ;i<spriteSheets.size();i++) {

        YAML::Node _spriteConfig = spriteSheets[i];

        auto id = _spriteConfig["id"].as<std::string>();
        auto path = _spriteConfig["path"].as<std::string>();
        auto scalingFactor = _spriteConfig["scaling-factor"].as<int>();
        auto spriteSliceSize =  _parseToVec2D_Int(_spriteConfig["slice-size"]);

        ssm->insertSpriteSheet(id, context, path, spriteSliceSize, scalingFactor );

    }

}

Vec2D_Float GameLevelConfigReader::_parseToVec2D_Float(YAML::Node node){
    return {
        .x = node["x"].as<float>(),
        .y = node["y"].as<float>()
    };
}

Vec2D_Int GameLevelConfigReader::_parseToVec2D_Int(YAML::Node node){
    return {
        .x = node["x"].as<int>(),
        .y = node["y"].as<int>()
    };
}

SDL_Color GameLevelConfigReader::_parseToSDL_Color(YAML::Node node){
    return {
        .r = node["r"].as<unsigned char>(),
        .g = node["g"].as<unsigned char>(),
        .b = node["b"].as<unsigned char>()
    };
}

