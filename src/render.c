#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <internal/abstrShader.h>
#include <internal/stlRead.h>

#include <cglm/cglm.h>
#include <math.h>
#include <stdint.h>

static unsigned vertexShader;
static unsigned fragmentShader;
static unsigned shaderProgram;

static unsigned VBO, VAO;

static uint32_t numTriangles;

void renderInit(const char *vertexSourceFile, const char* fragSourceFile)
{
	vertexShader = abstrShaderConstruct(GL_VERTEX_SHADER, vertexSourceFile);
	fragmentShader = abstrShaderConstruct(GL_FRAGMENT_SHADER, fragSourceFile);

	shaderProgram = abstrShaderProgramConstruct(
		2,
		(unsigned[]){vertexShader, fragmentShader}
	);

	float *vertices;
	stlRead("teapot.stl", &numTriangles, &vertices);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, numTriangles*9*sizeof(float), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	glBindVertexArray(0); 
	free(vertices);

	// uncomment this call to draw in wireframe polygons.
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void renderLoop(float aspectRatio)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Build transformation matrix.
	float timeValue = glfwGetTime();
	float period = 4.0f; // seconds
	float theta = 2.0f * M_PI * (1.0f/period) * timeValue;
	float scale = 0.25f;

	mat4 modelMat;
	glm_mat4_identity(modelMat);
	glm_translate_y(modelMat, -0.5f);
	glm_translate_z(modelMat, -2.0f);
	glm_rotate_x(modelMat, glm_rad(15.0f), modelMat);
	glm_rotate_y(modelMat, theta, modelMat);
	glm_scale_uni(modelMat, scale);

	mat4 perspectiveMat;
	glm_perspective(glm_rad(45.0f), aspectRatio, 0.1f, 100.0f, perspectiveMat);

	mat4 transMat;
	glm_mat4_mul(perspectiveMat, modelMat, transMat);

	int transLocation = glGetUniformLocation(shaderProgram, "transMat");

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);

	glUniformMatrix4fv(transLocation, 1, GL_FALSE, (float*)transMat);

	glDrawArrays(GL_TRIANGLES, 0, 3*numTriangles);
	glBindVertexArray(0);
}

void renderTerminate(void)
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
}
