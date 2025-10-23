#pragma once
#include "renderer.hpp"
#include <SDL2/SDL.h>

float forward, back, left, right, up, down = {0.0f};


int screenHeight = 480;
int screenWidth  = 640;


float camSpeed = 10.0f;


int wrapX=0;
//global mouse position with wrap around
float mouseX=0, mouseY=0;
//mouse pos on window 
int mouseWindowX = 0, mouseWindowY = 0;

bool paused = false;

float scrollPos = 10 * 10;

glm::vec2 KeyRot {0};

void handleUserInput(SDL_Window* window, bool &WindowClose){
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type){
            case SDL_QUIT:
                WindowClose = true;
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym){
                    case SDLK_w:
                        forward = 0.0f; break;
                    case SDLK_s:
                        back = 0.0f; break;
                    case SDLK_a:
                        left = 0.0f; break;
                    case SDLK_d:    
                        right = 0.0f; break;
                    case SDLK_SPACE:
                        up = 0.0f; break;
                    case SDLK_LSHIFT:
                        down = 0.0f; break;
                    case SDLK_p:
                        paused ^= 1; break;
                    case SDLK_UP:
                        KeyRot.x = 0; break;
                    case SDLK_DOWN:
                        KeyRot.x = 0; break;
                    case SDLK_LEFT:
                        KeyRot.y = 0; break;
                    case SDLK_RIGHT:
                        KeyRot.y = 0; break;
                }
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym){
                    case SDLK_w:
                        forward = camSpeed; break;
                    case SDLK_s:
                        back = camSpeed; break;
                    case SDLK_a:
                        left = camSpeed; break;
                    case SDLK_d:
                        right = camSpeed; break;
                    case SDLK_SPACE:
                        up = camSpeed; break;
                    case SDLK_LSHIFT:
                        down = camSpeed; break;
                    case SDLK_UP:
                        KeyRot.x = -1; break;
                    case SDLK_DOWN:
                        KeyRot.x = 1; break;
                    case SDLK_LEFT:
                        KeyRot.y = -1; break;
                    case SDLK_RIGHT:
                        KeyRot.y = 1; break;
                }
                break;
            case SDL_MOUSEMOTION:
                if(paused) return;
                mouseWindowX = event.motion.x;
                mouseWindowY = event.motion.y;
                if(mouseWindowX == 0) {
                    SDL_WarpMouseInWindow(window, screenWidth-2, mouseWindowY);
                    wrapX--;
                    mouseWindowX = screenWidth-2;
                }else if(mouseWindowX == screenWidth-1){
                    SDL_WarpMouseInWindow(window, 1, mouseWindowY);
                    wrapX++;
                    mouseWindowX = 1;
                }
                mouseX = mouseWindowX + (screenWidth-2) * wrapX;
                break;
                
            case SDL_MOUSEWHEEL:
                if(paused) return;
                scrollPos += event.wheel.preciseY;
                scrollPos = (scrollPos<1 ? 1 : scrollPos);
                camSpeed = (scrollPos) / 10;
                break;

            case SDL_WINDOWEVENT:
                if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    int newWidth  = event.window.data1;
                    int newHeight = event.window.data2;
                    glViewport(0, 0, (GLsizei) newWidth, (GLsizei) newHeight);
                    updateProjMatrix(window);
                }
                break;

        }
    }
}