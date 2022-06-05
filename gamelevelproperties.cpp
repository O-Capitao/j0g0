#include "gamelevel.hpp"

using namespace j0g0;

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
    SDL_Color color = {
        .r = node["r"].as<Uint8>(),
        .g = node["g"].as<Uint8>(),
        .b = node["b"].as<Uint8>(),
        .a = 255
    };

    return color;
}

std::vector<PlatformProperties> GameLevelConfigReader::generatePlatformProperties( std::string config_path ){

    YAML::Node levelProperties = YAML::LoadFile( config_path );
    YAML::Node platformsYaml = levelProperties["platforms"];
    std::vector<PlatformProperties> retVal;

    for (std::size_t i=0 ; i < platformsYaml.size(); i++) {

        YAML::Node curr_plat = platformsYaml[i];

        PlatformProperties _props = {
            .positionInWorld = _parseToVec2D_Float( curr_plat["position"]),
            .sizeInTiles = _parseToVec2D_Int(curr_plat["size"]),
            .spriteSheetId = curr_plat["sprite-sheet"].as<std::string>(),
            .tileMapSpriteSliceMatrix = curr_plat["tile-map"].as<std::vector<Uint8>>(),
            .sliceRotations_in90Deg = curr_plat["quarter-turns-ccw"].as<std::vector<Uint8>>(),
            .sliceFlip_H = curr_plat["flip-horizontal"].as<std::vector<bool>>(),
            .sliceFlip_V = curr_plat["flip-vertical"].as<std::vector<bool>>()
        };

        retVal.push_back( _props );
    }

    return retVal;
}