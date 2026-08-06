#pragma once

#include <glad/glad.h>

unsigned abstrShaderConstruct(GLenum shaderType, const char *srcFilename);

unsigned abstrShaderProgramConstruct(int count, unsigned *shaderIDs);

void abstrShaderSetUniform3fv(unsigned shaderProgram, const char *name, const float *value);

void abstrShaderSetUniformMatrix4fv(unsigned shaderProgram, const char *name, const float *value);
