#pragma once

#include "renderer.hpp"
#include "debug.hpp"
#include <vector>
#include <stdio.h>
#include <stdlib.h>

typedef struct Particle{
    float q;         //charge
    float m;         //mass
    glm::vec3 pos;   //position
    glm::vec3 vel;   //velocity
    glm::vec3 accel; //accel;
}__attribute__((packed)) Particle ;


class Entity_ParticleSim : public Entity{
public:
    Entity_ParticleSim(glm::vec3 pos, glm::vec3 scale) : Entity(){
        this->position = pos;
        this->scale = scale;
        if(cube == nullptr) initShaders();
    }

    void addParticle(Particle *p){
        if(ssboOk == true) return;
        particles.push_back(p);
    }

    void update(float timeStep){
        //init the buffer if needed
        if(ssboOk == false) updateSSBO();
        
        glUseProgram(this->computeShaderID[0]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);
        glDispatchCompute(particles.size(), particles.size(), 1);
        
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glUseProgram(this->computeShaderID[1]);
        glUniform1f(this->shaderUniformLocations[4], timeStep);
        glDispatchCompute(particles.size(), 1, 1);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        glUseProgram(0);

    }

    void render() {
        //render particles
        glUseProgram(this->renderShaderID);
        this->cube->getVertexBuffer()->bind();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);
        glUniformMatrix4fv(this->shaderUniformLocations[0],   1, GL_FALSE, &projectionMatrix[0][0]);
        glUniformMatrix4fv(this->shaderUniformLocations[1],   1, GL_FALSE, &viewMatrix[0][0]);
        glUniform3fv(this->shaderUniformLocations[2], 1, &this->position.x);
        glUniform3fv(this->shaderUniformLocations[3], 1, &this->scale.x);
        glDrawElementsInstanced(GL_TRIANGLES, this->cube->getVertexBuffer()->size(), GL_UNSIGNED_INT, 0, particles.size());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        this->cube->getVertexBuffer()->unbind();
        glUseProgram(0);
        //render bounding box
        renderBoundingBox(this->position, this->scale, 0);
    }

private: 
    //init of Shader and Mesh
    inline static Mesh* cube = nullptr;
    inline static GLuint computeShaderID[2];
    inline static GLuint renderShaderID;

    void initShaders(){
        cube = new Mesh("assets/obj/cube.obj");
        if(cube == nullptr){
            std::cout << "Error loading particle mesh" << std::endl;
        }

        computeShaderID[0] = shader_make_compute("shader/particle/particle.comp");
        computeShaderID[1] = shader_make_compute("shader/particle/particle_move.comp");
        renderShaderID  = shader_make("shader/particle/particle.vs", "shader/particle/particle.fs");
        if(computeShaderID[0] == 0 || computeShaderID[1] == 0 || renderShaderID == 0){
            std::cout << "Error loading particle shaders" << std::endl;
        }

        this->shaderUniformLocations = new int[5];
        //render shader
        shaderUniformLocations[0] = glGetUniformLocation(renderShaderID, "u_projection");
        shaderUniformLocations[1] = glGetUniformLocation(renderShaderID, "u_view");
        shaderUniformLocations[2] = glGetUniformLocation(renderShaderID, "u_pos");
        shaderUniformLocations[3] = glGetUniformLocation(renderShaderID, "u_scale");
        //move shader
        shaderUniformLocations[4] = glGetUniformLocation(computeShaderID[1], "timeStep");
    }

    bool ssboOk = false;
    void updateSSBO(){
        if(ssboOk == true) return;
        int dataSize = particles.size()*11;

        //setup ssbo
        glGenBuffers(1, &particleBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);
        //glBufferData(GL_SHADER_STORAGE_BUFFER, dataSize * sizeof(float), (void*) particleData, GL_DYNAMIC_DRAW);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, dataSize * sizeof(float), nullptr, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT );
        particleData = (float*) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, dataSize * sizeof(float), GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
        if(particleData == nullptr) {
            std::cout << "SSBO could not be mapped!!!!" << std::endl;
        }
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,0);

        //clean Buffer

        updateParitcleData();

        ssboOk = true;
    }

    void updateParitcleData(){
        for(int i=0; i<particles.size(); i++){
            Particle* p = particles[i];
            particleData[i*11+0] = p->q;
            particleData[i*11+1] = p->m;

            particleData[i*11+2] = p->pos.x;
            particleData[i*11+3] = p->pos.y;
            particleData[i*11+4] = p->pos.z;

            particleData[i*11+5] = p->vel.x;
            particleData[i*11+6] = p->vel.y;
            particleData[i*11+7] = p->vel.z;

            //accell buffer used for calc
            particleData[i*11+8]  = 1;
            particleData[i*11+9]  = 1;
            particleData[i*11+10] = 1;
        }
    }

    void update_particles_from_particleData(){
        for(int i=0; i<particles.size(); i++){
            Particle* p = particles[i];
            p->pos.x = particleData[i*11+2];
            p->pos.y = particleData[i*11+3];
            p->pos.z = particleData[i*11+4];
        }
    }


    GLuint particleBuffer;
    std::vector<Particle*> particles;
    float* particleData = nullptr;
};
