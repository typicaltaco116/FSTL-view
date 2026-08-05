#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <internal/abstrShader.h>
#include <internal/stlRead.h>

#include <cglm/cglm.h>
#include <math.h>
#include <stdint.h>

static unsigned _vertexShader;
static unsigned _fragmentShader;
static unsigned _shaderProgram;

static unsigned _VBO, _VAO;

static uint32_t _numTriangles;

void renderInit(const char *stlFilename, const char *vertexSourceFile, const char* fragSourceFile)
{
	_vertexShader = abstrShaderConstruct(GL_VERTEX_SHADER, vertexSourceFile);
	_fragmentShader = abstrShaderConstruct(GL_FRAGMENT_SHADER, fragSourceFile);

	_shaderProgram = abstrShaderProgramConstruct(
		2,
		(unsigned[]){_vertexShader, _fragmentShader}
	);

	float *vertices;
	stlRead(stlFilename, &_numTriangles, &vertices);

	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);
	glBindVertexArray(_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, _numTriangles*9*sizeof(float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	glBindVertexArray(0); 
	free(vertices);

	// uncomment this call to draw in wireframe polygons.
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void renderLoop(float aspectRatio, float deltaRad)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	static float theta = 0.0f;
	theta += deltaRad;

	mat4 modelMat;
	glm_mat4_identity(modelMat);
	glm_translate_y(modelMat, -0.5f);
	glm_translate_z(modelMat, -2.0f);
	glm_rotate_x(modelMat, glm_rad(15.0f), modelMat);
	glm_rotate_y(modelMat, theta, modelMat);
	glm_scale_uni(modelMat, 0.25f);

	mat4 perspectiveMat;
	glm_perspective(glm_rad(45.0f), aspectRatio, 0.1f, 100.0f, perspectiveMat);

	mat4 transMatrix;
	glm_mat4_mul(perspectiveMat, modelMat, transMatrix);

	glUseProgram(_shaderProgram);
	glBindVertexArray(_VAO);

	int transLocation = glGetUniformLocation(_shaderProgram, "transMat");
	glUniformMatrix4fv(transLocation, 1, GL_FALSE, (float*)transMatrix);

	glDrawArrays(GL_TRIANGLES, 0, 3*_numTriangles);
	glBindVertexArray(0);
}

void renderTerminate(void)
{
	glDeleteVertexArrays(1, &_VAO);
	glDeleteBuffers(1, &_VBO);
	glDeleteProgram(_shaderProgram);

	glfwTerminate();
}
