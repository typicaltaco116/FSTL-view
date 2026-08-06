#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <internal/abstrShader.h>
#include <internal/stlRead.h>

#include <cglm/cglm.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

// Scene parameters. (Global coords)
vec3 cameraPos = {0.0f, 0.75f, 2.0f};
vec3 cameraBoresight = {0.0f, 0.5f, 0.0f};
vec3 modelPos = {0.0f, 0.0f, 0.0f};
vec3 lightPos = {1.0f, 4.0f, 0.0f};
#define CAMERA_FOV_DEG (45.0f)

static unsigned _shaderProgram;
static unsigned _VAO;
static unsigned _VBO;
static uint32_t _numTriangles;
static float _scale;
static float _colorPeriod;

void renderInit(const char *stlFilename, const char *vertexSourceFile, const char *geoSourceFile, const char* fragSourceFile)
{
	unsigned vertexShader = abstrShaderConstruct(GL_VERTEX_SHADER, vertexSourceFile);
    unsigned geometryShader = abstrShaderConstruct(GL_GEOMETRY_SHADER, geoSourceFile);
	unsigned fragmentShader = abstrShaderConstruct(GL_FRAGMENT_SHADER, fragSourceFile);

	_shaderProgram = abstrShaderProgramConstruct(
		3,
		(unsigned[]){vertexShader, geometryShader, fragmentShader}
	);

	// Open STL and extract normals and vertices.
	stl_data_t stl = stlRead(stlFilename);
    free(stl.normals); // dump normals because they are usually bad ):
	_numTriangles = stl.polyCount;

	// Find maximum coordinate component.
	float maxCoord = 0.0f;
	for (int i = 0; i < _numTriangles * 9; ++i) {
		float component = fabsf(stl.vertices[i]);
		if (component > maxCoord)
			maxCoord = component;
	}
    // Force max coordinate of the model to be 1.0 in global units.
	_scale = (1.0f / maxCoord);
	_colorPeriod = maxCoord / 6.0f;

    // Generate and bind VAO.
	glGenVertexArrays(1, &_VAO);
	glBindVertexArray(_VAO);

    // Generate VBOs.
    glGenBuffers(1, &_VBO);
    unsigned vertexVBO = _VBO;

    // Allocate and copy vertices to VBO.
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, stl.polyCount*9*sizeof(float), stl.vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
    free(stl.vertices);

    // Unbind VAO.
	glBindVertexArray(0); 

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
    glm_translate(modelMat, modelPos);
	glm_rotate_x(modelMat, vertTheta, modelMat);
	glm_rotate_y(modelMat, horzTheta, modelMat);
	glm_scale_uni(modelMat, _scale);

    mat4 viewMat;
    glm_mat4_identity(viewMat);
    glm_lookat(cameraPos, cameraBoresight, GLM_YUP, viewMat);

	mat4 perspectiveMat;
	glm_perspective(glm_rad(CAMERA_FOV_DEG), aspectRatio, 0.1f, 100.0f, perspectiveMat);

	glUseProgram(_shaderProgram);
	glBindVertexArray(_VAO);

    // Vertex shader uniforms.
    int modelLocation = glGetUniformLocation(_shaderProgram, "model");
    int viewLocation = glGetUniformLocation(_shaderProgram, "view");
    int projectionLocation = glGetUniformLocation(_shaderProgram, "projection");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (float*)modelMat);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (float*)viewMat);
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, (float*)perspectiveMat);

    // Fragment shader uniforms.
    int lightPosLocation = glGetUniformLocation(_shaderProgram, "lightPos");
    glUniform3fv(lightPosLocation, 1, (float*)lightPos);

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
