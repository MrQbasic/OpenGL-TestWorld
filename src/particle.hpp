#pragma once
#include "renderer.hpp"
#include "debug.hpp"
#include <vector>


typedef struct Particle{
    float q;        //charge
    float m;        //mass
    glm::vec3 pos;  //position
    glm::vec3 vel;  //velocity
}__attribute__((packed)) Particle ;


class Entity_ParticleSim : public Entity{
public:
    Entity_ParticleSim(glm::vec3 pos, glm::vec3 scale) : Entity(){
        this->position = pos;
        this->scale = scale;
        if(cube == nullptr) initShaders();
    }

    void addParticle(Particle *p){
        particles.push_back(p);
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
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
        this->cube->getVertexBuffer()->unbind();
        glUseProgram(0);
        //render bounding box
        renderBoundingBox(this->position, this->scale, 0);
    }

private: 
    //init of Shader and Mesh
    inline static Mesh* cube = nullptr;
    static GLuint computeShaderID;
    static GLuint renderShaderID;
     
    void initShaders(){
        cube = new Mesh("assets/obj/cube.obj");
        computeShaderID = shader_make_compute("shader/particle/particle.comp");
        renderShaderID  = shader_make("shader/particle/particle.vs", "shader/particle/particle.fs");
        if(cube == nullptr || computeShaderID == 0 || renderShaderID == 0){
            std::cout << "Error loading particle shaders/mesh" << std::endl;
        }
    }

    GLuint particleBuffer;
    std::vector<Particle *> particles;

};
