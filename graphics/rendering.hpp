#pragma once

#include "SDL.h"

#include "gamegeometry.hpp"

namespace j0g0 {

    struct CanvasProperties {

        Vec2D_Int size;
        int pixelRatio = 4;
        SDL_Color backgroundColor;

    };

    struct RenderingContext {

        RenderingContext(){};
        ~RenderingContext();

        void init( int windowWidth, int windowHeight, int _pixelRatio );

        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* canvas;

        CanvasProperties canvasProperties;

        // sets target to canvas, clears buffer
        void beginRenderStep();

        // upscale by pixelRatio times
        void endRenderStep();

        void setBackgroundColor(const SDL_Color& _color);
    };
}
