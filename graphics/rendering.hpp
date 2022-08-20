#pragma once

#include "SDL.h"

#include "gamegeometry.hpp"
#include "gameconfig.hpp"

namespace j0g0 {

    struct RenderingContext {

        RenderingContext(){};
        ~RenderingContext();

        void init( std::string pathToConfigFile );

        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* canvas;

        // CanvasProperties canvasProperties;
        GameWindowConfig config;

        // sets target to canvas, clears buffer
        void beginRenderStep();

        // upscale by pixelRatio times
        void endRenderStep();

        void setBackgroundColor(const SDL_Color& _color);
    };
}
