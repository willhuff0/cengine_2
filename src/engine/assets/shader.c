//
// Created by wehuf on 9/27/2024.
//

#include "shader.h"

#include "../assets.h"

static void printShaderInfoLog(const GLuint shader, const GLenum type, const char* path) {
    GLint infoLogLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

    char* buffer = malloc(infoLogLength);
    glGetShaderInfoLog(shader, infoLogLength, NULL, buffer);

    fprintf(stderr, "[SHADER] Failed to compile %s shader (%s): %s\n", type == GL_VERTEX_SHADER ? "vertex" : "fragment", path, buffer);
    free(buffer);
}

static void printProgramInfoLog(const GLuint program) {
    GLint infoLogLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

    char* buffer = malloc(infoLogLength);
    glGetProgramInfoLog(program, infoLogLength, NULL, buffer);

    fprintf(stderr, "[SHADER] Failed to link program: %s\n", buffer);
    free(buffer);
}

static bool compileShader(const GLuint shader, const GLenum type, const char* path) {
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
        printShaderInfoLog(shader, type, path);
        return false;
    }

    return true;
}

static bool createShader(GLuint* shader, const char* path, const GLenum type) {
    *shader = glCreateShader(type);

    char* source = readFile(path);
    glShaderSource(*shader, 1, &source, NULL);
    free(source);

    if (compileShader(*shader, type, path)) {
        return true;
    } else {
        glDeleteShader(*shader);
        return false;
    }
}

static void lookupShaderUniformLocations(Shader* shader) {
    int numUniforms;
    glGetProgramiv(shader->program, GL_ACTIVE_UNIFORMS, &numUniforms);

    for (unsigned int i = 0; i < numUniforms; ++i) {
        int nameLength;
        glGetActiveUniformsiv(shader->program, 1, &i, GL_UNIFORM_NAME_LENGTH, &nameLength);

        char* nameBuffer = malloc(nameLength * sizeof(char));
        int length, size;
        GLuint type;
        glGetActiveUniform(shader->program, i, nameLength, &length, &size, &type, nameBuffer);
        const GLint location = glGetUniformLocation(shader->program, nameBuffer);
        shput(shader->uniformLocations, nameBuffer, location);
    }
}

ShaderID createShader(const char* vertexPath, const char* fragmentPath) {
    GLuint vertShader;
    if (!createShader(&vertShader, vertexPath, GL_VERTEX_SHADER)) {
        return -1;
    }

    GLuint fragShader;
    if (!createShader(&fragShader, fragmentPath, GL_FRAGMENT_SHADER)) {
        glDeleteShader(vertShader);
        return -1;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glDetachShader(program, vertShader);
    glDetachShader(program, fragShader);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        printProgramInfoLog(program);
        glDeleteProgram(program);
        return -1;
    }

    Shader shader { program, NULL };

    shdefault(shader.uniformLocations, -1);
    lookupShaderUniformLocations(&shader);

    arrput(assets.shaders, shader);
    return arrlen(assets.shaders) - 1;
}

void freeShader(const Shader* shader) {
    glDeleteProgram(shader->program);
}

Shader* lookupShader(const ShaderID id) {
    return &assets.shaders[id];
}

void bindShader(const ShaderID id) {
    glUseProgram(lookupShader(id)->program);
}

static GLint getUniformLocation(const ShaderID id, const char* name) {
    const GLint location = shget(lookupShader(id)->uniformLocations, name);
    if (location == -1) {
        fprintf(stderr, "[SHADER] Uniform with name '%s' does not exist\n", name);
        abort();
    }
    return location;
}

void setUniformBool  (const ShaderID id, const char* name, const bool  value) { setUniformInt(id, name, value ? 1 : 0); }
void setUniformInt   (const ShaderID id, const char* name, const int   value) { glUniform1i(getUniformLocation(id, name), value); }
void setUniformFloat (const ShaderID id, const char* name, const float value) { glUniform1f(getUniformLocation(id, name), value); }
void setUniformVec2  (const ShaderID id, const char* name, const vec2  value) { glUniform2fv(getUniformLocation(id, name), 1, value); }
void setUniformVec3  (const ShaderID id, const char* name, const vec3  value) { glUniform3fv(getUniformLocation(id, name), 1, value); }
void setUniformVec4  (const ShaderID id, const char* name, const vec4  value) { glUniform4fv(getUniformLocation(id, name), 1, value); }
void setUniformMat4  (const ShaderID id, const char* name, const mat4  value) { glUniformMatrix4fv(getUniformLocation(id, name), 1, GL_FALSE, (const float*)value); }