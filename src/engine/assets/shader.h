//
// Created by wehuf on 9/27/2024.
//

#ifndef SHADER_H
#define SHADER_H

#include "../../common.h"
#include "../../paths.h"

#define SHADER_DIR ASSETS_DIR "shaders" DIR

#define SHADER_SIMPLE                    SHADER_DIR "simple.vert",           SHADER_DIR "simple.frag", true
#define DEFAULT_SHADER_UBER              SHADER_DIR "uber.vert",             SHADER_DIR "uber.frag",   true

typedef int ShaderID;

typedef struct {
    char* key;
    GLint value;
} UniformLocation;

typedef struct {
    GLuint program;
    UniformLocation* uniformLocations;
} Shader;

// Adds a shader to assets and returns its ID
// Returns -1 if an error occurs
ShaderID createShaderFromPaths(const char* vertexPath, const char* fragmentPath, const bool bindCengineUbo);

void freeShader(const Shader* shader);

Shader* lookupShader(const ShaderID id);

void bindShader(const ShaderID id);

void setUniformBool  (const ShaderID id, const char* name, const bool  value);
void setUniformInt   (const ShaderID id, const char* name, const int   value);
void setUniformFloat (const ShaderID id, const char* name, const float value);
void setUniformVec2  (const ShaderID id, const char* name, const vec2  value);
void setUniformVec3  (const ShaderID id, const char* name, const vec3  value);
void setUniformVec4  (const ShaderID id, const char* name, const vec4  value);
void setUniformMat4  (const ShaderID id, const char* name, const mat4  value);

void setUniformBoolAt  (const GLint location, const bool  value);
void setUniformIntAt   (const GLint location, const int   value);
void setUniformFloatAt (const GLint location, const float value);
void setUniformVec2At  (const GLint location, const vec2  value);
void setUniformVec3At  (const GLint location, const vec3  value);
void setUniformVec4At  (const GLint location, const vec4  value);
void setUniformMat4At  (const GLint location, const mat4  value);

#endif //SHADER_H
