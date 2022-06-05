#pragma once

#include <vector>
#include <string>

#include <SDL.h>
#include <yaml-cpp/yaml.h>

#include "graphics/gamegeometry.hpp"
#include "graphics/viewport.hpp"
#include "graphics/spritesheet.hpp"

#include "gameobjs/platform.hpp"


namespace j0g0 {

    struct GameLevelProperties {

        std::string title;
        Vec2D_Float worldSize;
        SDL_Color backgroundColor;
    
    };

    struct GameLevelConfigReader {
        
        GameLevelProperties buildGameLevelProperties( std::string config_path );
        
        void addLevelSpritesToManager( 
            std::string config_path,
            RenderingContext *context,
            SpriteSheetManager *ssm
        );

        std::vector<PlatformProperties> generatePlatformProperties( std::string config_path );

        private:
        
            std::string _configFilePath;
            
            Vec2D_Float _parseToVec2D_Float(YAML::Node node);
            Vec2D_Int _parseToVec2D_Int(YAML::Node node);
            SDL_Color _parseToSDL_Color(YAML::Node node);
            
            Platform _parseToPlatform(YAML::Node node);

    };

    struct GameLevel {

        GameLevel( 
            std::string levelPropsFile, 
            RenderingContext* _context,
            SpriteSheetManager *ssm
        );

        ~GameLevel();

        void render();
        void update();

        void handleEvents( const SDL_Event& event );

        private:

            RenderingContext* _context_p;
            SpriteSheetManager *_ssm_p;
            GameLevelProperties _properties;
            ViewPort _viewport;
            GameLevelConfigReader _reader;

            std::vector<Platform*> _platforms_p_vec;
    };
}