#pragma once
#include "renderer.hpp"

/*
mode:
- bit 0:    enable outline;
- bit 1:    enable transparent Side
*/

Mesh* cube;
GLuint debugShader;
int debugShaderLocation_Projection;
int debugShaderLocation_View      ;
int debugShaderLocation_BoxSize   ;
int debugShaderLocation_BoxPos    ;
int debugShaderLocation_SunPos    ;

void setupDebug(){
    cube = new Mesh("assets/obj/cube.obj");
    debugShader = shader_make("shader/debug/boundingBox.vs", "shader/debug/boundingBox.fs");
    debugShaderLocation_Projection = glGetUniformLocation(debugShader, "u_projection");
    debugShaderLocation_View       = glGetUniformLocation(debugShader, "u_view");
    debugShaderLocation_BoxSize    = glGetUniformLocation(debugShader, "u_BoxSize");
    debugShaderLocation_BoxPos     = glGetUniformLocation(debugShader, "u_BoxPos");
    debugShaderLocation_SunPos     = glGetUniformLocation(debugShader, "u_SunPos");
}


void renderBoundingBox(glm::vec3 &boxPos, glm::vec3 &boxSize, int mode){
    glUseProgram(debugShader);
    cube->getVertexBuffer()->bind();
    glUniform3fv(debugShaderLocation_BoxPos , 1 , &boxPos.x);
    glUniform3fv(debugShaderLocation_BoxSize, 1 , &boxSize.x);
    glUniform3fv(debugShaderLocation_SunPos , 1 , &sunPos.x);
    glUniformMatrix4fv(debugShaderLocation_Projection,   1, GL_FALSE, &projectionMatrix[0][0]);
    glUniformMatrix4fv(debugShaderLocation_View,         1, GL_FALSE, &viewMatrix[0][0]);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);    //so we can see the other side facing half of the wireframe
    glDrawElements(GL_TRIANGLES, cube->getVertexBuffer()->size(), GL_UNSIGNED_INT, 0);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    cube->getVertexBuffer()->unbind();
    glUseProgram(0);
}