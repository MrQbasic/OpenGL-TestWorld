#include "renderer.hpp"
#pragma once


bool isSetUp = false;


Mesh*  planeMandelbrot;
GLuint shaderMandelbrot; 

int mandelbrotShaderLocations[5];

void setupMandelbrot(){
    shaderMandelbrot = shader_make("shader/mandelbrot/mandelbrot.vs", "shader/mandelbrot/mandelbrot.fs");
    planeMandelbrot = new Mesh("assets/obj/plane.obj");
    mandelbrotShaderLocations[0] = glGetUniformLocation(shaderMandelbrot, "u_projection");
    mandelbrotShaderLocations[1] = glGetUniformLocation(shaderMandelbrot, "u_view");
    mandelbrotShaderLocations[2] = glGetUniformLocation(shaderMandelbrot, "u_posShift");
    mandelbrotShaderLocations[3] = glGetUniformLocation(shaderMandelbrot, "u_size");
    mandelbrotShaderLocations[4] = glGetUniformLocation(shaderMandelbrot, "u_rot");
    isSetUp = true;
}

void renderMandelbrot(glm::vec3 pos, glm::vec3 size, glm::vec3 rot){
    if(!isSetUp) setupMandelbrot();

    glUseProgram(shaderMandelbrot);
    planeMandelbrot->getVertexBuffer()->bind();
    glUniformMatrix4fv(mandelbrotShaderLocations[0],     1, GL_FALSE, &projectionMatrix[0][0]);
    glUniformMatrix4fv(mandelbrotShaderLocations[1],     1, GL_FALSE, &viewMatrix[0][0]);
    glUniform3fv(mandelbrotShaderLocations[2],           1, &pos.x);
    glUniform3fv(mandelbrotShaderLocations[3],           1, &size.x);
    glUniform3fv(mandelbrotShaderLocations[4],           1, &rot.x);
    glDrawElements(GL_TRIANGLES, planeMandelbrot->getVertexBuffer()->size(), GL_UNSIGNED_INT, 0);
    planeMandelbrot->getVertexBuffer()->unbind();
    glUseProgram(0);
}