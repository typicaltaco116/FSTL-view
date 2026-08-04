#pragma once

#include <glad/glad.h>

unsigned abstrShaderConstruct(GLenum shaderType, const char *srcFilename);

unsigned abstrShaderProgramConstruct(int count, unsigned *shaderIDs);
