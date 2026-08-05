// Primary Header
#include <internal/abstrShader.h>

// OpenGL API Headers
#include <glad/glad.h>

// Stdandard Headers
#include <stdio.h>
#include <stdlib.h>


unsigned abstrShaderConstruct(GLenum shaderType, const char *srcFilename)
{
	FILE *fid = fopen(srcFilename, "r");

	size_t bufSize = 16;
	char *buffer = (char*)malloc(bufSize);

	int c = 0;
	size_t count = 0;
	while (c != EOF) {
		c = fgetc(fid);

		// Enlarge buffer if required.
		if (count + 1 > bufSize) {
			bufSize += 16;
			buffer = (char*)realloc(buffer, bufSize);
		}

		buffer[count++] = (char)c;
	}
	// replace EOF with NULL.
	buffer[count - 1] = '\0';

	unsigned shaderID = glCreateShader(shaderType);
	glShaderSource(shaderID, 1, (const char**)&buffer, NULL);
	free(buffer);

	glCompileShader(shaderID);
	int success;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
		printf("abstrShader: ERROR SHADER COMPILATION_FAILED\n%s\n", infoLog);
	}

	return shaderID;
}

unsigned abstrShaderProgramConstruct(int count, unsigned *shaderIDs)
{
	unsigned programID = glCreateProgram();
	
	// Stage all shaders for linking.
	for (int i = 0; i < count; ++i)
		glAttachShader(programID, shaderIDs[i]);

	glLinkProgram(programID);
	int success;
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(programID, 512, NULL, infoLog);
		printf("abstrShader: ERROR SHADER LINKING FAILED\n%s\n", infoLog);
	}

	// Free all shaders.
	for (int i = 0; i < count; ++i)
		glDeleteShader(shaderIDs[i]);

	return programID;
}
