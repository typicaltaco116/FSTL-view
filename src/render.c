#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <internal/abstrShader.h>
#include <internal/stlRead.h>

#include <cglm/cglm.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

static unsigned _shaderProgram;

static unsigned _VAO;
static unsigned _VBOs[2];

static uint32_t _numTriangles;
static float _scale;
static float _colorPeriod;

void renderInit(const char *stlFilename, const char *vertexSourceFile, const char* fragSourceFile)
{
	unsigned vertexShader = abstrShaderConstruct(GL_VERTEX_SHADER, vertexSourceFile);
	unsigned fragmentShader = abstrShaderConstruct(GL_FRAGMENT_SHADER, fragSourceFile);

	_shaderProgram = abstrShaderProgramConstruct(
		2,
		(unsigned[]){vertexShader, fragmentShader}
	);

	// Open STL and extract normals and vertices.
	stl_data_t stl = stlRead(stlFilename);
	_numTriangles = stl.polyCount;

	// Find maximum coordinate component.
	float maxCoord = 0.0f;
	for (int i = 0; i < _numTriangles * 9; ++i) {
		float component = fabsf(stl.vertices[i]);
		if (component > maxCoord)
			maxCoord = component;
	}
	_scale = (1.0f / maxCoord) * 0.9f;
	_colorPeriod = maxCoord / 6.0f;

    // Generate and bind VAO.
	glGenVertexArrays(1, &_VAO);
	glBindVertexArray(_VAO);

    // Generate VBOs.
    glGenBuffers(2, _VBOs);
    unsigned vertexVBO = _VBOs[0];
    unsigned normalVBO = _VBOs[1];

    // Allocate and copy vertices to VBO.
	glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
	glBufferData(GL_ARRAY_BUFFER, stl.polyCount*9*sizeof(float), stl.vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
    free(stl.vertices);

    // Duplicate each normal for normal buffer to be same size as vertices.
    float *dupNormals = (float*)malloc(stl.polyCount*9*sizeof(float));
    for (int i = 0; i < stl.polyCount; ++i) {
        float *dstPtr = dupNormals + i * 9;
        memcpy(dstPtr + 0, stl.normals + i * 3, 3 * sizeof(float));
        memcpy(dstPtr + 3, stl.normals + i * 3, 3 * sizeof(float));
        memcpy(dstPtr + 6, stl.normals + i * 3, 3 * sizeof(float));
    }
    free(stl.normals);

    // Allocate and copy normals to VBO.
    glGenBuffers(1, &normalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, stl.polyCount*9*sizeof(float), dupNormals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    free(dupNormals);

    // Unbind VAO.
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

    mat4 viewMat;
    glm_mat4_identity(viewMat);

	mat4 perspectiveMat;
	glm_perspective(glm_rad(45.0f), aspectRatio, 0.1f, 100.0f, perspectiveMat);

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
	int colorPeriodLocation = glGetUniformLocation(_shaderProgram, "colorPeriod");
	glUniform1f(colorPeriodLocation, _colorPeriod);

	glDrawArrays(GL_TRIANGLES, 0, 3*_numTriangles);
	glBindVertexArray(0);
}

void renderTerminate(void)
{
	glDeleteVertexArrays(1, &_VAO);
	glDeleteBuffers(2, _VBOs);
	glDeleteProgram(_shaderProgram);

	glfwTerminate();
}
