#include <iostream>

#include "gameconfig.hpp"


using namespace j0g0;

GameLevelProperties GameConfigReader::buildGameLevelProperties( std::string config_path ){

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

void GameConfigReader::addLevelSpritesToManager( 
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

Vec2D_Float GameConfigReader::_parseToVec2D_Float(YAML::Node node){

    Vec2D_Float out;
    // return {
        out.x = node["x"].as<float>();//,
        out.y = node["y"].as<float>();
    // };
    return out;
}

Vec2D_Int GameConfigReader::_parseToVec2D_Int(YAML::Node node){
    return {
        .x = node["x"].as<int>(),
        .y = node["y"].as<int>()
    };
}

SDL_Color GameConfigReader::_parseToSDL_Color(YAML::Node node){

    SDL_Color color = {
        .r = node["r"].as<Uint8>(),
        .g = node["g"].as<Uint8>(),
        .b = node["b"].as<Uint8>(),
        .a = 255
    };

    return color;
}

SDL_Rect GameConfigReader::_parseToSDL_Rect(YAML::Node node){
    SDL_Rect rect = {
        .x = node["x"].as<int>(),
        .y = node["y"].as<int>(),
        .w = node["w"].as<int>(),
        .h = node["h"].as<int>(),
    };

    return rect;
}

std::vector<PlatformProperties> GameConfigReader::generatePlatformProperties( std::string config_path ){

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
            .sliceFlip_V = curr_plat["flip-vertical"].as<std::vector<bool>>(),
            .ellasticCoef = curr_plat["ellastic-coef"].as<float>(),
            .frictionCoef = curr_plat["friction-coef"].as<float>(),
        };

        retVal.push_back( _props );
    }

    return retVal;
}

ViewPortProperties GameConfigReader::generateViewPortProperties( std::string config_path ){

    YAML::Node lvlProps = YAML::LoadFile( config_path );
    YAML::Node vpYaml = lvlProps["viewport"];

    ViewPortProperties vp_conf = 
    {
        .width = vpYaml["width"].as<float>(),
        .worldPosition = _parseToVec2D_Float( vpYaml["position"] )
    };

    return vp_conf;
}

std::vector<ActorProperties> GameConfigReader::generateActorProperties( std::string config_path ){
    
    YAML::Node lvlProps = YAML::LoadFile( config_path );
    YAML::Node actors = lvlProps["actors"];
    std::vector<ActorProperties> props;

    // for (std::size_t i = 0; i < actors.size(); i++) {
    for (auto currActor : actors){
        
        ActorProperties actor = {
            .id = currActor["id"].as<std::string>(),
            .spriteSheetId = currActor["sprite-sheet-id"].as<std::string>(),
            .initialPositionInWorld = _parseToVec2D_Float( currActor["initial-position"] ),
            .boundingBox_SliceCoordinates = _parseToSDL_Rect( currActor["bounding-box"] ),
            .idleAnimationId = currActor["idle-animation-id"].as<std::string>(),
            .mass = currActor["mass"].as<float>(),
            .frictionCoef = currActor["friction-coef"].as<float>(),
        };

        actor.spriteAnimations = _generateSpriteAnimationProperties_ForActorProperties( currActor["sprite-animations"] );
        props.push_back(actor);
    }

    return props;
}

std::vector<SpriteAnimationProperties> GameConfigReader::_generateSpriteAnimationProperties_ForActorProperties( YAML::Node animations ){
    std::vector<SpriteAnimationProperties> retval;

    for (YAML::Node currAnimation: animations){
        SpriteAnimationProperties spriteAnimation = {
            .id = currAnimation["id"].as<std::string>(),
            .size = currAnimation["size"].as<Uint8>(),
            .sliceIndexes = currAnimation["slice-indexes"].as<std::vector<Uint8>>(),
            .sliceDurations_ms = currAnimation["slice-durations"].as<std::vector<Uint16>>(),
        };

        retval.push_back(spriteAnimation);
    }

    return retval;
}


