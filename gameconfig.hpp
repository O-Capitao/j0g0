#pragma once

#include <string>
#include "yaml-cpp/yaml.h"
#include "SDL.h"

#include "graphics/rendering.hpp"
#include "graphics/spritesheet.hpp"
#include "graphics/gamegeometry.hpp"


namespace j0g0 {

    struct GameLevelProperties {
        std::string title;
        Vec2D_Float worldSize;
        SDL_Color backgroundColor;
    };

    struct PlatformProperties{

        Vec2D_Float positionInWorld;
        Vec2D_Int sizeInTiles;
        std::string spriteSheetId;
        std::vector<Uint8> tileMapSpriteSliceMatrix;
        // ranging from 0 - 180. For larger values use flip
        std::vector<Uint8> sliceRotations_in90Deg;
        std::vector<bool>sliceFlip_H;
        std::vector<bool>sliceFlip_V;

        float ellasticCoef, frictionCoef;

    };

    struct ViewPortProperties{
        Vec2D_Float worldPosition;
        float width;
    };

    struct SpriteAnimationProperties {
        std::string id;
        std::string spriteSheetId;
        Uint8 size;
        std::vector<Uint8> sliceIndexes;
        std::vector<Uint16> sliceDurations_ms;
        Uint16 uniformSliceDuration_ms = 100;
    };
    
    struct ActorProperties {

        std::string id;
        std::string spriteSheetId;

        Vec2D_Float initialPositionInWorld;
        SDL_Rect boundingBox_SliceCoordinates;

        std::vector<SpriteAnimationProperties> spriteAnimations;
        std::string idleAnimationId;

        float mass;
        float frictionCoef;
    };
    
    
    
    struct GameConfigReader {
        
        GameLevelProperties buildGameLevelProperties( std::string config_path );
        
        void addLevelSpritesToManager( 
            std::string config_path,
            RenderingContext *context,
            SpriteSheetManager *ssm
        );

        std::vector<PlatformProperties> generatePlatformProperties( std::string config_path );
        ViewPortProperties generateViewPortProperties( std::string config_path );
        std::vector<ActorProperties> generateActorProperties( std::string config_path );
        
        private:
        
            std::string _configFilePath;
            Vec2D_Float _parseToVec2D_Float(YAML::Node node);
            Vec2D_Int _parseToVec2D_Int(YAML::Node node);
            SDL_Color _parseToSDL_Color(YAML::Node node);
            SDL_Rect _parseToSDL_Rect(YAML::Node node);

            std::vector<SpriteAnimationProperties> _generateSpriteAnimationProperties_ForActorProperties( YAML::Node animations );

    };

}