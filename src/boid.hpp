#pragma once
#include <CL/cl.h>
#include "renderer.hpp"
#include "debug.hpp"
#include <vector>


class Entity_BoidSim : public Entity{
private:
    //optim so we dont have to read in the mesh so often TODO maybe do this on a global scale in the entity constructor ?
    inline static Mesh* boidMesh = nullptr;
    static GL_Buffer_Vertex* getVertexBuffer(){
        if(boidMesh == nullptr) boidMesh = new Mesh("assets/obj/arrow.obj");
        return boidMesh->getVertexBuffer();
    }
    //same with shaders. only compile once
    inline static GLuint computeShaderPreload=0, renderShaderPreload=0;
    
    static void getShaders(GLuint* computeShader, GLuint* renderShader){
        if(computeShaderPreload == 0) computeShaderPreload = shader_make_compute("shader/boid/boid.comp");
        if(renderShaderPreload  == 0) renderShaderPreload  = shader_make("shader/boid/boid.vs", "shader/boid/boid.fs");
        *computeShader = computeShaderPreload; 
        *renderShader  = renderShaderPreload;
    }

public:
    Entity_BoidSim(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale, int boidCount) : Entity(){
        //copy some stuff
        this->position = pos;
        this->rotation = rot;
        this->scale = scale; 
        this->boidCount = boidCount;
        
        //Mesh
        this->vertexBuffer = getVertexBuffer();
        
        //setup Shaders
        getShaders(&(this->computeShaderID),&(this->shaderID));
        this->shaderUniformLocations = new int[5];
        shaderUniformLocations[0] = glGetUniformLocation(this->shaderID, "u_projection");
        shaderUniformLocations[1] = glGetUniformLocation(this->shaderID, "u_view");
        shaderUniformLocations[2] = glGetUniformLocation(this->shaderID, "u_cagePos");
        shaderUniformLocations[3] = glGetUniformLocation(this->shaderID, "u_cageSize");;
        shaderUniformLocations[4] = glGetUniformLocation(this->computeShaderID, "u_cageSize");

        //setup shared buffer between render and compute
        std::vector<float> boidData;
        for(int i=0; i<boidCount; i++){
            //position
            boidData.push_back((float)rand()/(float)RAND_MAX * this->scale.x);
            boidData.push_back((float)rand()/(float)RAND_MAX * this->scale.y);
            boidData.push_back((float)rand()/(float)RAND_MAX * this->scale.z);
            //vel
            boidData.push_back((float)rand()/(float)RAND_MAX*0.01-0.005);
            boidData.push_back((float)rand()/(float)RAND_MAX*0.01-0.005);
            boidData.push_back((float)rand()/(float)RAND_MAX*0.01-0.005);
    
        }

        //setup shared Buffer
        glGenBuffers(1, &boidBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, boidBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, boidData.size() * sizeof(float), (void*) boidData.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,0);
    }

    void update(){
        glUseProgram(this->computeShaderID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, boidBuffer);
        glUniform3fv(shaderUniformLocations[4], 1, &scale.x);
        glDispatchCompute(this->boidCount, 1, 1);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        glUseProgram(0);
    }

    void render() {
        glUseProgram(this->shaderID);
        this->vertexBuffer->bind();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, boidBuffer);
        glUniformMatrix4fv(this->shaderUniformLocations[0],   1, GL_FALSE, &projectionMatrix[0][0]);
        glUniformMatrix4fv(this->shaderUniformLocations[1],   1, GL_FALSE, &viewMatrix[0][0]);
        glUniform3fv(this->shaderUniformLocations[2], 1, &this->position.x);
        glUniform3fv(this->shaderUniformLocations[3], 1, &this->scale.x);
        glDrawElementsInstanced(GL_TRIANGLES, this->vertexBuffer->size(), GL_UNSIGNED_INT, 0, boidCount);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        this->vertexBuffer->unbind();
        glUseProgram(0);
        renderBoundingBox(this->position, this->scale, 0);
    }

protected:
    GLuint computeShaderID;
    GLuint boidBuffer;
    int boidCount;

};