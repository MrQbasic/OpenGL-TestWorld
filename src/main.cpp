#define GLEW_STATIC
#define SDL_MAIN_HANDLED
#define STB_IMAGE_IMPLEMENTATION
#define GLCALL(call) call

#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>


#include "stb_image.h"
#include "glm/glm.hpp"
#include "randHelper.hpp"

#include "input.hpp"
#include "renderer.hpp"
#include "background.hpp"
#include "boid.hpp"
#include "mandelBrot.hpp"
#include "particle.hpp"

SDL_Window* init(const char* title, int width, int height, int windowflags){
    SDL_Window* window;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 2);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    //SDL_ShowCursor(SDL_DISABLE);

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowflags);
    return window;
}


void GLAPIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                              GLsizei length, const GLchar* message, const void* userParam) {
    std::cout << "OpenGL Debug Output: " << message << std::endl;
}
void setupOpenGLDebugging() {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugCallback, nullptr);
}


int main(){
    //init
    SDL_Window* window = init("Test",screenWidth,screenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    renderInit(window, "shader/shader.vs", "shader/shader.fs");

    //main Loop
    bool close = false;

    setupOpenGLDebugging();
    setupDebug();
    
    Texture brick("assets/texture/Brick.png");
    Texture grass("assets/texture/Grass.png");

    Mesh m("./assets/obj/test.obj");
    Mesh m_cube("./assets/obj/cube.obj");

    EntityGroup eg;
    for(int i=0; i<10000; i++){
        Entity*e = NULL;
        if(randFloat(-1.0f, 1.0f) >= 0.0f){
            e = new Entity(m_cube, brick.getTextureID());
        }else{
            e = new Entity(m, grass.getTextureID());
        }   
        eg.add(*e);
        e->setPosition(randFloat(-10.f, 10.0f), randFloat(0.f, 20.0f), randFloat(40.f, 60.0f));
        e->setRotation(randFloat(-10.f, 10.0f), randFloat(-10.f, 10.0f), randFloat(-10.f, 10.0f));
        e->setScale(0.5f, 0.5f, 0.5f);
    }
    

    Mesh teapot("./assets/obj/teapot.obj");
    EntityGroup teapots;
    for(int x=0; x<4; x++){
        for(int y=0; y<4; y++){
            for(int z=0; z<4; z++){
                Entity *e = new Entity(teapot, brick.getTextureID());
                e->setPosition(x*1-14, z*1+4, y*1+4);
                e->setRotation(0, 0, 0);
                e->setScale(0.03, 0.03, 0.03);
                teapots.add(*e);
            }
        }
    }


    Entity_BoidSim boidSim(glm::vec3(-5,0,20), glm::vec3(0), glm::vec3(20), 80);
    Entity_BoidSim boidSim2(glm::vec3(20,0,20), glm::vec3(0), glm::vec3(40), 160);


    int cnt = 3;
    float sizeDist = 0.1f;
    glm::vec3 size = glm::vec3(15);
    Entity_ParticleSim particleSim(glm::vec3(-35,0,20), size);
    for(int x=0; x<cnt; x++){
        for(int y=0; y<cnt; y++){
            for(int z=0; z<cnt; z++){
                struct Particle *e = new struct Particle();
                e->pos.x = (sizeDist / (cnt+1)) * (x+1) -(sizeDist/2);
                e->pos.y = (sizeDist / (cnt+1)) * (y+1) -(sizeDist/2);
                e->pos.z = (sizeDist / (cnt+1)) * (z+1) -(sizeDist/2);
                e->q = randFloat(-1, 1);
                e->m = 100.0f;
                e->vel = glm::vec3(0);
                particleSim.addParticle(e);
            }
        }
    }


    float c=0.0f;

    glm::vec3 camPos(0, 10, 0);
    glm::vec2 camRot(0.0f);
    glm::vec2 keyRotCnt {0};
    SDL_WarpMouseInWindow(window, 200, 400);

    Uint64 timeNow = SDL_GetPerformanceCounter();
    Uint64 timeLast = 0;
    double dt;


    double speed = 0.005;
    double speed_rot= 0.01;

    int frame = 0;

    while(!close){
        frame++;

        //calculate delta time
        timeNow = SDL_GetPerformanceCounter();
        dt = (double)((timeNow - timeLast)*1000 / (double)SDL_GetPerformanceFrequency() );
        c += 0.01f * dt;
        if(frame % 100 == 0) std::cout << "renderTime: " << dt << " fps: " << 1000/dt << std::endl;

        //check user inputs
        handleUserInput(window, close);

        //cam movement
        camRot.y = mouseX * speed_rot;
        camRot.x = ((float)mouseWindowY / 480.0f - 0.5) * 3.1415 ;
        keyRotCnt += KeyRot * 0.001f * (float)dt;
        camRot += keyRotCnt;
        //apply cam movement
        float dx = (back - forward);
        float dz = (left - right); 
        float dy = (up - down);
        camPos.x += (+dx * speed * dt * sin(camRot.y)) + (+dz * speed * dt * cos(camRot.y));
        camPos.z += (-dx * speed * dt * cos(camRot.y)) - (-dz * speed * dt * sin(camRot.y));
        camPos.y += dy * speed * dt;



        //reset buffer
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //calculate mapping matricies
        setCameraTranslation(camPos, camRot);

        //do sim updates
        boidSim.update();
        boidSim2.update();

        //rotate the teapots
        for(int i=0; i<teapots.entities->size(); i++){
            teapots.entities->at(i)->setRotation(0, c , 0);
        }


        particleSim.update(0.01);
        
        
        //render everything
        renderBackground(camPos);
        
        //eg.render();
        renderMandelbrot(glm::vec3(5, 10, -20), glm::vec3(10), glm::vec3(-0.5, 0, 0));
        teapots.render();
        boidSim.render();
        boidSim2.render();
        
        particleSim.render();
        
        timeLast = timeNow;
        //swap buffers
        SDL_GL_SwapWindow(window);
    }
    return 0;
}
