#pragma once

#include <string>
#include "yaml-cpp/yaml.h"
#include "SDL.h"
#include "graphics/gamegeometry.hpp"


namespace j0g0 {


// Classes that hold the properties contained in the input .yaml file.
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
    struct GameWindowConfig {
        Vec2D_Int canvasSize;
        Vec2D_Int windowSize;
        int pixel_ratio;
        SDL_Color backgroundColor;
        std::string windowTitle;
    };


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
    struct GameWorldConfig {
        // std::string title;
        Vec2D_Float worldSize;
        // SDL_Color backgroundColor;
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

    struct PlatformKeyPositionConfig{
        Vec2D_Float point;
        float timeArrival_s;
        float timeRest_s;
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
    struct PlatformConfig{
        std::string id;
        Vec2D_Float positionInWorld;
        Vec2D_Int sizeInTiles;
        std::string spriteSheetId;
        std::vector<Uint8> tileMapSpriteSliceMatrix;

        // ranging from 0 - 180. For larger values use flip
        std::vector<Uint8> sliceRotations_in90Deg;
        std::vector<bool>sliceFlip_H;
        std::vector<bool>sliceFlip_V;
        
        std::vector<PlatformKeyPositionConfig> keyPositions_vec;
        
        float ellasticCoef = 0, frictionCoef = 0;
        bool isMovingPlatform = false;

        Vec2D_Int fillDirection;
        SDL_Color fillColor;

        int offsetMinus = 0, offsetPlus = 0;
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
    struct ViewPortConfig{
        Vec2D_Float worldPosition;
        float width;
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
    struct SpriteSheetConfig {
        std::string id;
        std::string path;
        int scaleFactor = 1;
        Vec2D_Int sliceSize;
    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
    struct SpriteAnimationConfig {
        std::string id;
        std::string spriteSheetId;
        Uint8 size;
        std::vector<Uint8> sliceIndexes;
        std::vector<Uint16> sliceDurations_ms;
        Uint16 uniformSliceDuration_ms = 100;
        
        bool isTransient;
        std::string transientFrom;
        std::string transientTo;
    };
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
    struct ActorConfig {

        std::string id;
        std::string spriteSheetId;

        Vec2D_Float initialPositionInWorld;
        Vec2D_Float initialVelocity;
        SDL_Rect boundingBox_SliceCoordinates;

        std::vector<SpriteAnimationConfig> spriteAnimations;
        std::string idleAnimationId;

        float mass;
        float frictionCoef;

        float jumpDvY;
        float walkSpeed;

        bool isPlayer;
        bool isBounceable;


    };

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
    struct BackgroundConfig{
        bool isTiled;

        // in file:
        std::string id;
        std::string spriteSheetId;

        // controls the overlaying of different backgrounds.
        // Higher z_index is drawn closer to the foreground.
        int z_index;

        // The background will move with the Viewport's velocity
        // affected by a factor -> usually smaller than 1.
        Vec2D_Float ratioToViewportVelocity_pixelPerSec;

        // a constant velocity, added to the Viewport's factored parcel
        Vec2D_Float ownVelocity_pixelPerSec;

        // Position of the Top Left Corner
        Vec2D_Int positionInCanvas;

        // represent theBackground as a Tiled surface, just like the Platform
        Vec2D_Int sizeInTiles;
        std::vector<Uint8> tileMapSpriteSliceMatrix;
        std::vector<Uint8> sliceRotations_in90Deg;
        std::vector<bool>sliceFlip_H;
        std::vector<bool>sliceFlip_V;
        
        // for some infinite scrolling
        bool repeatX = false;
        bool repeatY = false;

    };
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////    
    struct PropConfig {

        std::string id;
        std::string spriteSheetId;
        
        // "static", "animated" or "tiled"
        std::string prop_type = "static";
        Vec2D_Float positionInWorld;

        int spriteSliceIndex = 0;
        SpriteAnimationConfig animation;

        bool destroyable = false;
        int hp = 0;

        int z_index = 1;
    };
    
    
    struct GameConfigReader {
        
        GameWorldConfig readGameLevelConfig( std::string config_path );
        GameWindowConfig readGameWindowConfig( std::string config_path );
        std::vector<SpriteSheetConfig> readSpriteSheetConfig( std::string config_path );
        std::vector<PlatformConfig> readPlatformConfig( std::string config_path );
        ViewPortConfig readViewPortConfig( std::string config_path );
        std::vector<ActorConfig> readActorConfig( std::string config_path );
        std::vector<BackgroundConfig> readBackgroundConfig( std::string config_path );
        std::vector<PropConfig> readPropConfig( std::string config_path );

        private:
        
            std::string _configFilePath;
            Vec2D_Float _parseToVec2D_Float(YAML::Node node);
            Vec2D_Int _parseToVec2D_Int(YAML::Node node);
            SDL_Color _parseToSDL_Color(YAML::Node node);
            SDL_Rect _parseToSDL_Rect(YAML::Node node);
            PlatformKeyPositionConfig _parsePlatformKeyPositionConfig( YAML::Node node );
            std::vector<SpriteAnimationConfig> _readSpriteAnimationConfig_ForActorConfig( YAML::Node animations );

    };

}