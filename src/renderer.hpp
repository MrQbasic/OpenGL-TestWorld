#pragma once

#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "buffer.hpp"
#include "input.hpp"

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <array>
#include <initializer_list>

#define GLCALL(call) call

glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;

glm::vec3 cam_pos;
glm::vec3 cam_rot;

glm::vec3 sunPos;

GLuint entity_shaderID;
int entity_location_projectionMatrix;
int entity_location_viewMatrix;
int entity_location_transformationMatrix;
int entity_location_texture;

std::string shader_parse(const char *filename);
GLuint shader_compile(std::string source, GLenum type);
GLuint shader_make(const char *vs, const char *fs);
void createProjMatrix(float fov, float far, float near, SDL_Window *window);
void updateProjMatrix(SDL_Window *window);

bool renderInit(SDL_Window *window, const char *e_vs, const char *e_fs)
{
    GLenum glewErr = glewInit();

    entity_shaderID = shader_make(e_vs, e_fs);
    entity_location_projectionMatrix = GLCALL(glGetUniformLocation(entity_shaderID, "u_projection"));
    entity_location_viewMatrix = GLCALL(glGetUniformLocation(entity_shaderID, "u_view"));
    entity_location_transformationMatrix = GLCALL(glGetUniformLocation(entity_shaderID, "u_transform"));
    entity_location_texture = GLCALL(glGetUniformLocation(entity_shaderID, "u_texture"));

    updateProjMatrix(window);

    viewMatrix = glm::mat4(1.0f);

    sunPos = glm::vec3(5000, 2000, 200);

    glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    stbi_set_flip_vertically_on_load(true);

    std::cout << "Renderer OK" << std::endl;
    return true;
}

void setCameraTranslation(glm::vec3 camPos, glm::vec2 camRot)
{
    viewMatrix = glm::mat4(1.0f);
    viewMatrix = glm::rotate(viewMatrix, camRot.x, glm::vec3(1.0f, 0.0f, 0.0f));
    viewMatrix = glm::rotate(viewMatrix, camRot.y, glm::vec3(0.0f, 1.0f, 0.0f));
    viewMatrix = glm::translate(viewMatrix, camPos * glm::vec3(1, -1, 1));
}

GLuint shader_make_compute(const char *shaderfile)
{
    std::string source = shader_parse(shaderfile);
    if (source == "")
    {
        return 0;
    }
    GLuint shader = shader_compile(source, GL_COMPUTE_SHADER);
    if (shader == 0)
    {
        return 0;
    }

    GLuint shaderID = glCreateProgram();
    glAttachShader(shaderID, shader);
    glLinkProgram(shaderID);
    glDetachShader(shaderID, shader);
    glDeleteShader(shader);
    // check linking
    int res;
    glGetProgramiv(shaderID, GL_LINK_STATUS, &res);
    if (res != GL_TRUE)
    {
        int lenght = 0;
        glGetProgramiv(shaderID, GL_INFO_LOG_LENGTH, &lenght);
        char *msg = new char[lenght];
        glGetProgramInfoLog(shaderID, lenght, &lenght, msg);
        std::cout << "Shader compile error: " << msg << std::endl;
        delete[] msg;
        return 0;
    }
    return shaderID;
}

GLuint shader_make(const char *vs, const char *fs)
{
    std::string VS_source = shader_parse(vs);
    std::string FS_source = shader_parse(fs);
    if (VS_source == "" || FS_source == "")
    {
        return 0;
    }
    GLuint VS = shader_compile(VS_source, GL_VERTEX_SHADER);
    GLuint FS = shader_compile(FS_source, GL_FRAGMENT_SHADER);
    if (VS == 0 || FS == 0)
    {
        return 0;
    }
    GLuint shaderID = glCreateProgram();
    glAttachShader(shaderID, VS);
    glAttachShader(shaderID, FS);
    glLinkProgram(shaderID);
    glDetachShader(shaderID, VS);
    glDetachShader(shaderID, FS);
    glDeleteShader(VS);
    glDeleteShader(FS);
    // check linking
    int res;
    glGetProgramiv(shaderID, GL_LINK_STATUS, &res);
    if (res != GL_TRUE)
    {
        int lenght = 0;
        glGetProgramiv(shaderID, GL_INFO_LOG_LENGTH, &lenght);
        char *msg = new char[lenght];
        glGetProgramInfoLog(shaderID, lenght, &lenght, msg);
        std::cout << "Shader compile error: " << msg << std::endl;
        delete[] msg;
        return 0;
    }
    return shaderID;
}

std::string shader_parse(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file == nullptr)
    {
        std::cout << "File " << filename << " no found" << std::endl;
        return "";
    }
    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file);
    rewind(file);
    std::string cont;
    cont.resize(filesize);
    fread(&cont[0], 1, filesize, file);
    fclose(file);
    return cont;
}

GLuint shader_compile(std::string source, GLenum type)
{
    GLuint id = glCreateShader(type);
    
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, 0);
    glCompileShader(id);
    int res;
    glGetShaderiv(id, GL_COMPILE_STATUS, &res);
    if (res != GL_TRUE)
    {
        int lenght = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &lenght);
        char *msg = new char[lenght];
        glGetShaderInfoLog(id, lenght, &lenght, msg);
        std::cout << "Shader compile error: " << msg << std::endl;
        delete[] msg;
        return 0;
    }

    return id;
}

void createProjMatrix(float fov, float far, float near, SDL_Window *window)
{
    int sw, sh;
    SDL_GetWindowSizeInPixels(window, &sw, &sh);
    float a = (float)sw / (float)sh;
    float ys = (1.0f / glm::tan(glm::radians(fov / 2.0f))) * a;
    float xs = ys / a;
    float fl = far - near;
    projectionMatrix[0][0] = xs;
    projectionMatrix[1][1] = ys;
    projectionMatrix[2][2] = -(far + near) / fl;
    projectionMatrix[2][3] = -1.0f;
    projectionMatrix[3][2] = -(2 * near * far) / fl;
}

void updateProjMatrix(SDL_Window *window)
{
    createProjMatrix(70.0f, 1000.0f, 0.01f, window);
}

class Mesh
{
public:
    Mesh(const char *filename)
    {
        v_buffer = new std::vector<Vertex>;
        std::vector<std::array<float, 3>> vert;
        std::vector<std::array<float, 2>> text;
        std::vector<std::array<float, 3>> norm;
        std::string line;
        std::ifstream file(filename);
        i_buffer = new std::vector<int>;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::vector<std::string> input;
            for (std::string s; iss >> s;)
            {
                input.push_back(s);
            }
            if (input.size() == 0)
                continue;
            if (input[0] == "v")
            {

                std::array<float, 3> x{std::stof(input[1]), std::stof(input[2]), std::stof(input[3])};
                vert.push_back(x);
            }
            else if (input[0] == "vt")
            {
                std::array<float, 2> x{std::stof(input[1]), std::stof(input[2])};
                text.push_back(x);
            }
            else if (input[0] == "vn")
            {
                std::array<float, 3> x{std::stof(input[1]), std::stof(input[2]), std::stof(input[3])};
                norm.push_back(x);
            }
            else if (input[0] == "f")
            {
                for (int i = 1; i <= 3; i++)
                {
                    std::vector<std::string> buffer;
                    Vertex v;
                    uint16_t i2 = 0;
                    uint16_t i3 = 0;
                    std::string x;
                    input[i] += '/';
                    while (true)
                    {
                        if (i2 >= input[i].size())
                        {
                            v_buffer->push_back(v);
                            break;
                        }
                        char c = input[i][i2];
                        i2++;
                        if (c != '/')
                        {
                            x += c;
                        }
                        else
                        {
                            i3++;
                            try{
                            switch (i3)
                            {
                            case 1:
                                v.x = vert[stoi(x) - 1][0];
                                v.y = vert[stoi(x) - 1][1];
                                v.z = vert[stoi(x) - 1][2];
                                // i_buffer->push_back(stoi(x)-1);
                                break;
                            case 2:
                                v.u = text[stoi(x) - 1][0];
                                v.v = text[stoi(x) - 1][1];
                                break;
                            case 3:
                                // read in normals
                                v.nx = norm[stoi(x) - 1][0];
                                v.ny = norm[stoi(x) - 1][1];
                                v.nz = norm[stoi(x) - 1][2];
                                break;
                            }
                            }catch(const std::exception_ptr& e){
                            }
                            x.clear();
                            if (i3 == 3)
                            {
                                v_buffer->push_back(v);
                                i2 = 0;
                                i3 = 0;
                                break;
                            }
                        }
                    }
                }
            }
        }
        // check for duplicates
        i_buffer = new std::vector<int>;
        for (int i1 = 0; i1 < v_buffer->size(); i1++)
        {
            Vertex v = v_buffer->at(i1);
            i_buffer->push_back(i1);
            for (int i2 = (i1 - 1); i2 >= 0; i2--)
            {
                if (cmpVertex(v, v_buffer->at(i2)))
                {
                    i_buffer->at(i1) = i2;
                }
            }
        }

        /*
            std::vector<Vertex> x;
            for(int i=0; i<vert.size(); i++){
                std::array<float, 3> val = vert[i];
                Vertex v;
                v.x = val[0];
                v.y = val[1];
                v.z = val[2];
                x.push_back(v);
            }
        */

        // create GL Buffers
        vertexBuffer = new GL_Buffer_Vertex(v_buffer, i_buffer);

        std::cout << "read in mesh: " << filename << std::endl;
        /*
        for(int i=0; i<v_buffer->size(); i++){
            std::cout << "Vertex: " << v_buffer->at(i).x << " " << v_buffer->at(i).y << " " << v_buffer->at(i).z << std::endl;
        }
        for(int i=0; i<i_buffer->size(); i++){
            std::cout << "index: " << i_buffer->at(i) << std::endl;
        }
        */
    }

    ~Mesh()
    {
        delete v_buffer;
        delete i_buffer;
        delete vertexBuffer;
    }
    GL_Buffer_Vertex *getVertexBuffer()
    {
        return vertexBuffer;
    }

private:
    GL_Buffer_Vertex *vertexBuffer;
    std::vector<int> *i_buffer;
    std::vector<Vertex> *v_buffer;
};

class Entity
{
protected:
    Entity()
    {
    }

public:
    Entity(Mesh &m, GLuint textureID, GLuint shaderID = entity_shaderID)
    {
        GL_Buffer_Vertex *v = m.getVertexBuffer();
        this->vertexBuffer = v;
        this->textureID = textureID;
        this->shaderID = shaderID;
        this->shaderUniformLocations = new int[3];
        // yes you have to think to mod this go cry abt it
        shaderUniformLocations[0] = glGetUniformLocation(this->shaderID, "u_projection");
        shaderUniformLocations[1] = glGetUniformLocation(this->shaderID, "u_view");
        shaderUniformLocations[2] = glGetUniformLocation(this->shaderID, "u_transform");
    }
    // yes we do out clean up stuff... sometimes
    ~Entity()
    {
        delete this->shaderUniformLocations;
    }

    void setPosition(float x, float y, float z)
    {
        position = glm::vec3(x, y, z);
    }
    void setRotation(float x, float y, float z)
    {
        rotation = glm::vec3(x, y, z);
    }
    void setScale(float x, float y, float z)
    {
        scale = glm::vec3(x, y, z);
    }

    // renders at given location;
    void render()
    {
        glUseProgram(this->shaderID);
        this->vertexBuffer->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->textureID);
        glUniformMatrix4fv(this->shaderUniformLocations[0], 1, GL_FALSE, &projectionMatrix[0][0]);
        glUniformMatrix4fv(this->shaderUniformLocations[1], 1, GL_FALSE, &viewMatrix[0][0]);
        setTransformation(position, rotation, scale);
        glUniformMatrix4fv(this->shaderUniformLocations[2], 1, GL_FALSE, &transformationMatrix[0][0]);
        glDrawElements(GL_TRIANGLES, this->vertexBuffer->size(), GL_UNSIGNED_INT, 0);
        this->vertexBuffer->unbind();
        glUseProgram(0);
    }

    glm::mat4 *getTransformationMatrix()
    {
        setTransformation(position, rotation, scale);
        return &transformationMatrix;
    }
    GL_Buffer_Vertex *getVertexBuffer()
    {
        return vertexBuffer;
    }
    GLuint getTextureID()
    {
        return textureID;
    }
    GLuint getShaderID()
    {
        return shaderID;
    }
    int *getShaderUniformLocation()
    {
        return shaderUniformLocations;
    }

protected:
    void setTransformation(glm::vec3 &position, glm::vec3 &rotation, glm::vec3 &scale)
    {
        // copy inputs into local vars
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;
        // reset matrix
        this->transformationMatrix = glm::mat4(1.0f);
        // translate matrix
        this->transformationMatrix = glm::translate(this->transformationMatrix, position);
        // rotate matrix
        this->transformationMatrix = glm::rotate(this->transformationMatrix, rotation.x, glm::vec3(1, 0, 0));
        this->transformationMatrix = glm::rotate(this->transformationMatrix, rotation.y, glm::vec3(0, 1, 0));
        this->transformationMatrix = glm::rotate(this->transformationMatrix, rotation.z, glm::vec3(0, 0, 1));
        // scale matrix
        this->transformationMatrix = glm::scale(this->transformationMatrix, scale);
    }

protected:
    // used by the shader
    GL_Buffer_Vertex *vertexBuffer;
    glm::mat4 transformationMatrix;
    GLuint textureID;
    GLuint shaderID;
    int *shaderUniformLocations;

    // specific to the entity
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

class Texture
{
public:
    Texture(const char *filename)
    {
        int w = 0;
        int h = 0;
        int bits = 0;
        auto textureBuffer = stbi_load(filename, &w, &h, &bits, 4);
        GLCALL(glGenTextures(1, &textureID));
        GLCALL(glBindTexture(GL_TEXTURE_2D, textureID));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer));
        GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
        if (textureBuffer)
        {
            stbi_image_free(textureBuffer);
        }
    }
    ~Texture()
    {
        GLCALL(glDeleteTextures(1, &textureID));
    }
    GLuint getTextureID()
    {
        return textureID;
    }

private:
    GLuint textureID;
};

// old DONT USE
class EntityGroup_L1
{
public:
    EntityGroup_L1()
    {
        entities = new std::vector<Entity *>;
        vertexBuffer = NULL;
    }
    ~EntityGroup_L1()
    {
        delete entities;
    }
    bool add(Entity &e)
    {
        if (e.getVertexBuffer() == NULL)
        {
            std::cout << "WARN: index/vertex buffer of Entity at " << static_cast<void *>(&e) << "invalid. Entity nit added to group!" << std::endl;
            return false;
        }
        if (vertexBuffer != NULL)
        {
            if (vertexBuffer != e.getVertexBuffer() || textureID != e.getTextureID())
            {
                std::cout << "WARN: Entity at " << static_cast<void *>(&e) << " index/vertex buffer or textureID mismatch. Entity not added to group!" << std::endl;
                return false;
            }
        }
        else
        {
            vertexBuffer = e.getVertexBuffer();
            textureID = e.getTextureID();
        }
        entities->push_back(&e);
        return true;
    }

    void render()
    {
        // check if there are entries
        if (vertexBuffer == NULL)
            return;
        // setup all GL stuff
        glUseProgram(entity_shaderID);
        vertexBuffer->bind();
        GLCALL(glActiveTexture(GL_TEXTURE0));
        GLCALL(glBindTexture(GL_TEXTURE_2D, textureID));
        GLCALL(glUniformMatrix4fv(entity_location_projectionMatrix, 1, GL_FALSE, &projectionMatrix[0][0]));
        GLCALL(glUniformMatrix4fv(entity_location_viewMatrix, 1, GL_FALSE, &viewMatrix[0][0]));
        GLCALL(glUniform1i(entity_location_transformationMatrix, 0));
        // iterate through entries
        GLsizei indexBufferSize = entities->at(0)->getVertexBuffer()->size();
        for (int i = 0; i < entities->size(); i++)
        {
            Entity *e = entities->at(i);
            GLCALL(glUniformMatrix4fv(entity_location_transformationMatrix, 1, GL_FALSE, &(*(e)->getTransformationMatrix())[0][0]));
            GLCALL(glDrawElements(GL_TRIANGLES, indexBufferSize, GL_UNSIGNED_INT, 0));
        }
        // undo GL stuff
        vertexBuffer->unbind();
        glUseProgram(0);
    }

public:
    std::vector<Entity *> *entities;

private:
    GL_Buffer_Vertex *vertexBuffer;
    GLuint textureID;
};

// old DONT USE
class EntityGroup_L2
{
public:
    EntityGroup_L2()
    {
        entities = new std::vector<Entity *>;
        vertexBuffer = NULL;
    }
    ~EntityGroup_L2()
    {
        delete entities;
    }
    bool add(Entity &e)
    {
        if (e.getVertexBuffer() == NULL)
        {
            std::cout << "WARN: index/vertex buffer of Entity at " << static_cast<void *>(&e) << "invalid. Entity nit added to group!" << std::endl;
            return false;
        }
        if (vertexBuffer != NULL)
        {
            if (vertexBuffer != e.getVertexBuffer())
            {
                std::cout << "WARN: Entity at " << static_cast<void *>(&e) << " index/vertex buffer or textureID mismatch. Entity not added to group!" << std::endl;
                return false;
            }
        }
        else
        {
            vertexBuffer = e.getVertexBuffer();
        }
        entities->push_back(&e);
        return true;
    }

    void render()
    {
        // check if there are entries
        if (vertexBuffer == NULL)
            return;
        // setup all GL stuff
        glUseProgram(entity_shaderID);
        vertexBuffer->bind();
        GLCALL(glActiveTexture(GL_TEXTURE0));
        GLCALL(glUniformMatrix4fv(entity_location_projectionMatrix, 1, GL_FALSE, &projectionMatrix[0][0]));
        GLCALL(glUniformMatrix4fv(entity_location_viewMatrix, 1, GL_FALSE, &viewMatrix[0][0]));
        GLCALL(glUniform1i(entity_location_transformationMatrix, 0));
        // iterate through entries
        GLsizei indexBufferSize = entities->at(0)->getVertexBuffer()->size();
        for (int i = 0; i < entities->size(); i++)
        {
            Entity *e = entities->at(i);
            GLCALL(glBindTexture(GL_TEXTURE_2D, e->getTextureID()));
            GLCALL(glUniformMatrix4fv(entity_location_transformationMatrix, 1, GL_FALSE, &(*(e)->getTransformationMatrix())[0][0]));
            GLCALL(glDrawElements(GL_TRIANGLES, indexBufferSize, GL_UNSIGNED_INT, 0));
        }
        // undo GL stuff
        vertexBuffer->unbind();
        glUseProgram(0);
    }

public:
    std::vector<Entity *> *entities;

private:
    GL_Buffer_Vertex *vertexBuffer;
};

// instance rendering
//- shader: fixed
//- model matrix: instanced
class EntityGroup
{
public:
    EntityGroup()
    {
        entities = new std::vector<Entity *>;
    }
    ~EntityGroup()
    {
        delete entities;
    }
    bool add(Entity &e)
    {
        entities->push_back(&e);
        return true;
    }
    void render()
    {
        // check if there are entries
        if (entities->size() == 0)
            return;
        // shader
        glUseProgram(entities->at(0)->getShaderID());
        // Vert buffer
        entities->at(0)->getVertexBuffer()->bind();
        // Texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, entities->at(0)->getTextureID());
        // cam mats
        int *uniforms = entities->at(0)->getShaderUniformLocation();
        glUniformMatrix4fv(uniforms[0], 1, GL_FALSE, &projectionMatrix[0][0]);
        glUniformMatrix4fv(uniforms[1], 1, GL_FALSE, &viewMatrix[0][0]);

        // setTransformation(position, rotation, scale);
        for (int i = 0; i < this->entities->size(); i++)
        {
            glUniformMatrix4fv(uniforms[2], 1, GL_FALSE, (const float *)this->entities->at(i)->getTransformationMatrix());
            glDrawElements(GL_TRIANGLES, entities->at(0)->getVertexBuffer()->size(), GL_UNSIGNED_INT, 0);
        }

        // Disconnec
        entities->at(0)->getVertexBuffer()->unbind();
        glUseProgram(0);
    }

private:
    std::vector<glm::mat4> transformMats;

public:
    std::vector<Entity *> *entities;
};