#pragma once
#include <cstddef>
#include <GL/glew.h>
#include <vector>

struct Vertex{
    //position data
    float x=0, y=0, z = 0;
    //texture coords
    float u=0, v=0;
    //normal coords
    float nx=0, ny=0, nz=0;
};

bool cmpVertex(Vertex &a, Vertex &b){
    if(a.x != b.x) return false;
    if(a.y != b.y) return false;
    if(a.z != b.z) return false;
    if(a.u != b.u) return false;
    if(a.v != b.v) return false;
    return true;
}

class GL_Buffer_Vertex {
public:
    GL_Buffer_Vertex(std::vector<Vertex> *vertices, std::vector<int> *indices) {
        //gen vao
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        //gen vbo
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices->size()*sizeof(Vertex), vertices->data(), GL_STATIC_DRAW);
        
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices->size()*sizeof(int), indices->data(), GL_STATIC_DRAW);
        // Step 2: Set up the vertex attributes (position)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
        //
        glBindVertexArray(0);
        length = indices->size();
    }
    ~GL_Buffer_Vertex(){
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteVertexArrays(1, &vao);
    }
    void bind() {
        glBindVertexArray(vao);
    }
    void unbind() {
        glBindVertexArray(0);
    }
    int size(){
        return this->length;
    }
private:
    GLuint vao, vbo, ebo;
    int length;
};