#include "game-defines.hpp"
#include "gameconfig.hpp"


using namespace j0g0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
GameLevelProperties GameConfigReader::readGameLevelProperties( std::string config_path ){

    YAML::Node levelProperties = YAML::LoadFile( config_path );

    auto worldSize = _parseToVec2D_Float(levelProperties["world-size"]);

    return {
        .worldSize = worldSize
    };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<SpriteSheetConfig> GameConfigReader::readSpriteSheetConfig( std::string config_path ){

    YAML::Node config = YAML::LoadFile( config_path );
    YAML::Node spriteSheets = config["sprite-sheets"];

    std::vector<SpriteSheetConfig> retval;

    if (auto spriteSheets = config["sprite-sheets"]){

        for (std::size_t i=0 ;i<spriteSheets.size();i++) {

            YAML::Node _spriteConfig = spriteSheets[i];

            retval.push_back({
                .id = _spriteConfig["id"].as<std::string>(),
                .path = _spriteConfig["path"].as<std::string>(),
                .scaleFactor = _spriteConfig["scaling-factor"].as<int>(),
                .sliceSize =  _parseToVec2D_Int(_spriteConfig["slice-size"])
            });

        }

    }

    return retval;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
Vec2D_Float GameConfigReader::_parseToVec2D_Float(YAML::Node node){

    Vec2D_Float out;
    // return {
        out.x = node["x"].as<float>();//,
        out.y = node["y"].as<float>();
    // };
    return out;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
Vec2D_Int GameConfigReader::_parseToVec2D_Int(YAML::Node node){
    return {
        .x = node["x"].as<int>(),
        .y = node["y"].as<int>()
    };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
SDL_Color GameConfigReader::_parseToSDL_Color(YAML::Node node){

    SDL_Color color = {
        .r = node["r"].as<Uint8>(),
        .g = node["g"].as<Uint8>(),
        .b = node["b"].as<Uint8>(),
        .a = 255
    };

    return color;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
SDL_Rect GameConfigReader::_parseToSDL_Rect(YAML::Node node){
    SDL_Rect rect = {
        .x = node["x"].as<int>(),
        .y = node["y"].as<int>(),
        .w = node["w"].as<int>(),
        .h = node["h"].as<int>(),
    };

    return rect;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
GameWindowConfig GameConfigReader::readGameWindowConfig( std::string config_path ){

    YAML::Node props = YAML::LoadFile( config_path );

    if (auto winConfig = props["window"] ){
        GameWindowConfig c = {
            .canvasSize = _parseToVec2D_Int( winConfig["canvas-size"] ),
            .pixel_ratio = winConfig["pixel-ratio"].as<int>(),
            .backgroundColor = _parseToSDL_Color(winConfig["background-color"]),
            .windowTitle = winConfig["title"].as<std::string>()
        };

        c.windowSize = {
            .x = c.canvasSize.x * c.pixel_ratio,
            .y = c.canvasSize.y * c.pixel_ratio
        };

        return c;
    } else {
        throw std::runtime_error("'window' config is reqired.");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<PlatformProperties> GameConfigReader::readPlatformProperties( 
    std::string config_path 
){
    #if DEBUG
    printf("entering platform propertes\n");
    #endif
    
    YAML::Node levelProperties = YAML::LoadFile( config_path );
    YAML::Node platformsYaml = levelProperties["platforms"];
    std::vector<PlatformProperties> retVal;

    for (std::size_t i=0 ; i < platformsYaml.size(); i++) {

        YAML::Node curr_plat = platformsYaml[i];

        PlatformProperties _props = {
            .id = curr_plat["id"].as<std::string>(),
            .positionInWorld = _parseToVec2D_Float( curr_plat["position"]),
            .sizeInTiles = _parseToVec2D_Int(curr_plat["size"]),
            .spriteSheetId = curr_plat["sprite-sheet"].as<std::string>(),
            .tileMapSpriteSliceMatrix = curr_plat["tile-map"].as<std::vector<Uint8>>(),
            .ellasticCoef = curr_plat["ellastic-coef"].as<float>(),
            .frictionCoef = curr_plat["friction-coef"].as<float>()
        };

        int tileCount = _props.tileMapSpriteSliceMatrix.size();

        // Optional properties
        if (auto qt = curr_plat["quarter-turns-ccw"]){
            _props.sliceRotations_in90Deg = curr_plat["quarter-turns-ccw"].as<std::vector<Uint8>>();
        } else {
            _props.sliceRotations_in90Deg = std::vector<Uint8>( tileCount, 0 ); //{ 0 , 0 , 0 };
        }

        if (auto fh = curr_plat["flip-horizontal"]){
            _props.sliceFlip_H = curr_plat["flip-horizontal"].as<std::vector<bool>>();
        } else {
            _props.sliceFlip_H = std::vector<bool>( tileCount, false );
        }

        if (auto fv = curr_plat["flip-vertical"]){
            _props.sliceFlip_V = fv.as<std::vector<bool>>();
        } else {
            _props.sliceFlip_V = std::vector<bool>( tileCount, false );
        }

        if (auto fillD = curr_plat["fill-direction"]){
            _props.fillDirection = _parseToVec2D_Int(fillD);
            
            if (auto fillC = curr_plat["fill-color"]){

                _props.fillColor = _parseToSDL_Color( fillC );

            } else {
                
                printf("Error Parsing Platform Properties for %s. A fill-color must also be specified in the config file.\n", _props.id.c_str());

                throw std::runtime_error(
                    "Parse Error"    
                );

            }

            if (auto offsetMin = curr_plat["offset-minus"]){
                _props.offsetMinus = offsetMin.as<int>();
            }

            if (auto offsetPlu = curr_plat["offset-plus"]){
                _props.offsetPlus = offsetPlu.as<int>();
            }


        }else{
            _props.fillDirection = {0,0};
        }

        


        // Optional - only for moving plats
        if (auto _keyPositions = curr_plat["key-positions"]){
            
            _props.isMovingPlatform = true;
           
            for (std::size_t i=0 ; i < _keyPositions.size(); i++) {

                YAML::Node curr_point = _keyPositions[i];

                _props.keyPositions_vec.push_back({
                    .point = _parseToVec2D_Float( curr_point["point"] ),
                    .timeArrival_s = curr_point["time-arrival"].as<float>(),
                    .timeRest_s = curr_point["time-rest"].as<float>(),
                });
            }
        }

        retVal.push_back( _props );
    }

    #if DEBUG
    printf("built platform propertes\n");
    #endif
    
    return retVal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
ViewPortProperties GameConfigReader::readViewPortProperties( std::string config_path ){

    YAML::Node lvlProps = YAML::LoadFile( config_path );
    YAML::Node vpYaml = lvlProps["viewport"];

    ViewPortProperties vp_conf = 
    {
        .width = vpYaml["width"].as<float>(),
        .worldPosition = _parseToVec2D_Float( vpYaml["position"] )
    };

    return vp_conf;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<ActorProperties> GameConfigReader::readActorProperties( std::string config_path ){
    
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
            .initialVelocity = {.x= 0, .y = 0},
            .isBounceable = true,
            .jumpDvY = currActor["jump-dvy"].as<float>(),
            .walkSpeed = currActor["walk-speed"].as<float>()
        };

        actor.spriteAnimations = _readSpriteAnimationProperties_ForActorProperties( currActor["sprite-animations"] );
        actor.isPlayer = actor.id == "player";
        
        // Optional properties
        if (auto initialVelocity = currActor["initial-velocity"]){
            actor.initialVelocity = _parseToVec2D_Float( currActor["initial-velocity"] );
        }
        
        props.push_back(actor);
    }

    return props;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<BackgroundProperties> GameConfigReader::readBackgroundProperties( std::string config_path ){

#if DEBUG
    printf("Entering readBackgroundProperties.\n");
#endif
    
    std::vector<BackgroundProperties> retVal;
    YAML::Node levelProperties = YAML::LoadFile( config_path );
    
    if (YAML::Node backgroundsYaml = levelProperties["backgrounds"]){

        

        for (std::size_t i=0 ; i < backgroundsYaml.size(); i++) {

            YAML::Node curr_bckgrnd = backgroundsYaml[i];

            BackgroundProperties _props = {
                .id = curr_bckgrnd["id"].as<std::string>(),
                .spriteSheetId = curr_bckgrnd["sprite-sheet"].as<std::string>(),
                .sizeInTiles = _parseToVec2D_Int(curr_bckgrnd["size"]),
                .tileMapSpriteSliceMatrix = curr_bckgrnd["tile-map"].as<std::vector<Uint8>>(),
                .ratioToViewportVelocity_pixelPerSec = _parseToVec2D_Float( curr_bckgrnd["vp-velocity-ratio"] ),
                .z_index = curr_bckgrnd["z-index"].as<int>()
            };

            int tileCount = _props.tileMapSpriteSliceMatrix.size();

            // Optional properties
            if (auto qt = curr_bckgrnd["quarter-turns-ccw"]){
                _props.sliceRotations_in90Deg = curr_bckgrnd["quarter-turns-ccw"].as<std::vector<Uint8>>();
            } else {
                _props.sliceRotations_in90Deg = std::vector<Uint8>( tileCount, 0 ); //{ 0 , 0 , 0 };
            }

            if (auto fh = curr_bckgrnd["flip-horizontal"]){
                _props.sliceFlip_H = curr_bckgrnd["flip-horizontal"].as<std::vector<bool>>();
            } else {
                _props.sliceFlip_H = std::vector<bool>( tileCount, false );
            }

            if (auto fv = curr_bckgrnd["flip-vertical"]){
                _props.sliceFlip_V = curr_bckgrnd["flip-vertical"].as<std::vector<bool>>();
            } else {
                _props.sliceFlip_V = std::vector<bool>( tileCount, false );
            }

            if (auto vel = curr_bckgrnd["velocity"]){
                _props.ownVelocity_pixelPerSec = _parseToVec2D_Float( curr_bckgrnd["velocity"] );
            } else {
                _props.ownVelocity_pixelPerSec = {0,0};
            }

            if (auto position = curr_bckgrnd["position-in-canvas"]){
                _props.positionInCanvas = _parseToVec2D_Int( curr_bckgrnd["position-in-canvas"] );
            }else{
                _props.positionInCanvas = {0,0};
            }
            if (auto repeat_x = curr_bckgrnd["repeat-x"]){
                _props.repeatX = repeat_x.as<bool>();
            }
            if (auto repeat_y = curr_bckgrnd["repeat-y"]){
                _props.repeatY = repeat_y.as<bool>();
            }

            retVal.push_back( _props );
        }

        #if DEBUG
        printf("Finished readBackgroundProperties\n");
        #endif
            
    }

    return retVal;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<SpriteAnimationProperties> GameConfigReader::_readSpriteAnimationProperties_ForActorProperties( YAML::Node animations ){
    std::vector<SpriteAnimationProperties> retval;

    for (YAML::Node currAnimation: animations){
        SpriteAnimationProperties spriteAnimation = {
            .id = currAnimation["id"].as<std::string>(),
            .size = currAnimation["size"].as<Uint8>(),
            .sliceIndexes = currAnimation["slice-indexes"].as<std::vector<Uint8>>(),
            .sliceDurations_ms = currAnimation["slice-durations"].as<std::vector<Uint16>>(),
            .isTransient = false
        };

        if (auto transientFromNode = currAnimation["transient-from"]){
            spriteAnimation.isTransient = true;
            spriteAnimation.transientFrom = transientFromNode.as<std::string>();
        }

        if (auto transientToNode = currAnimation["transient-to"]){
            spriteAnimation.isTransient = true;
            spriteAnimation.transientTo = transientToNode.as<std::string>();
        }

        retval.push_back(spriteAnimation);
    }

    return retval;
}


