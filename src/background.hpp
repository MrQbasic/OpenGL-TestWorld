#include "renderer.hpp"


bool inited = false;


GLuint backgroundShader = 0;
int shaderProjMatrix;
int shaderViewMatrix;
int shaderCamPos;
int shaderBgColor;
Mesh* backgroundGroundPlane;
//setup background Renderer
void renderBackgroundInit(){
    //shader
    backgroundShader = shader_make("shader/background/shaderBackground.vs", "shader/background/shaderBackground.fs");
    if(backgroundShader == 0){
        std::cout << "ERROR: Couldn't compile shaders for Background!" << std::endl; 
        return;
    }
    shaderProjMatrix = GLCALL(glGetUniformLocation(backgroundShader, "u_projection"));
    shaderViewMatrix = GLCALL(glGetUniformLocation(backgroundShader, "u_view"));
    shaderCamPos     = GLCALL(glGetUniformLocation(backgroundShader, "cameraPos"));
    shaderBgColor    = GLCALL(glGetUniformLocation(backgroundShader, "groundColor"));
    //buffers
    backgroundGroundPlane = new Mesh("assets/obj/plane.obj");
    inited = true;

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cout << "OpenGL Error in render setup: " << err << std::endl;
    }
}




void renderBackground(glm::vec3 camPos){
    if(!inited) renderBackgroundInit();
    if(!inited) return;
    
    glUseProgram(backgroundShader);
    backgroundGroundPlane->getVertexBuffer()->bind();
    GLCALL(glUniformMatrix4fv(shaderProjMatrix,     1, GL_FALSE, &projectionMatrix[0][0]));
    GLCALL(glUniformMatrix4fv(shaderViewMatrix,     1, GL_FALSE, &viewMatrix[0][0]));
    GLCALL(glUniform3f(shaderCamPos, camPos[0], camPos[1], camPos[2]) );
    GLCALL(glUniform3f(shaderBgColor, 0.2f, 0.2f, 0.2f) );
    GLCALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
    backgroundGroundPlane->getVertexBuffer()->unbind();
    glUseProgram(0);
}