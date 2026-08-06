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
static float _scale;
static float _colorPeriod;

void renderInit(const char *stlFilename, const char *vertexSourceFile, const char* fragSourceFile)
{
	_vertexShader = abstrShaderConstruct(GL_VERTEX_SHADER, vertexSourceFile);
	_fragmentShader = abstrShaderConstruct(GL_FRAGMENT_SHADER, fragSourceFile);

	_shaderProgram = abstrShaderProgramConstruct(
		2,
		(unsigned[]){_vertexShader, _fragmentShader}
	);

	// Open STL and extract normals and vertices.
	stl_data_t stl = stlRead(stlFilename);
	_numTriangles = stl.polyCount;

	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);
	glBindVertexArray(_VAO);

	// Allocate buffer but don't perform copy yet.
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, stl.polyCount*9*sizeof(float), stl.vertices, GL_STATIC_DRAW);

	// Find maximum coordinate component.
	float maxCoord = 0.0f;
	for (int i = 0; i < _numTriangles * 9; ++i) {
		float component = fabsf(stl.vertices[i]);
		if (component > maxCoord)
			maxCoord = component;
	}
	free(stl.vertices);
	free(stl.normals);

	_scale = (1.0f / maxCoord) * 0.9f;
	_colorPeriod = maxCoord / 6.0f;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	glBindVertexArray(0); 

	// uncomment this call to draw in wireframe polygons.
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
}

void renderLoop(float aspectRatio, float horzDeltaRad, float vertDeltaRad)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static float horzTheta = 0.0f;
	static float vertTheta = 0.0f;
	horzTheta += horzDeltaRad;
	vertTheta += vertDeltaRad;

	mat4 modelMat;
	glm_mat4_identity(modelMat);
	glm_translate_y(modelMat, -0.5f);
	glm_translate_z(modelMat, -2.0f);
	glm_rotate_x(modelMat, vertTheta, modelMat);
	glm_rotate_y(modelMat, horzTheta, modelMat);
	glm_scale_uni(modelMat, _scale);

	mat4 perspectiveMat;
	glm_perspective(glm_rad(45.0f), aspectRatio, 0.1f, 100.0f, perspectiveMat);

	mat4 transMatrix;
	glm_mat4_mul(perspectiveMat, modelMat, transMatrix);

	glUseProgram(_shaderProgram);
	glBindVertexArray(_VAO);

	int transLocation = glGetUniformLocation(_shaderProgram, "transMat");
	int colorPeriodLocation = glGetUniformLocation(_shaderProgram, "colorPeriod");
	glUniformMatrix4fv(transLocation, 1, GL_FALSE, (float*)transMatrix);
	glUniform1f(colorPeriodLocation, _colorPeriod);

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
