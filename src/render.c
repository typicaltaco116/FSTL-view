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

typedef struct {
    unsigned program;
    unsigned VAO;
    unsigned VBO;
} renderPipeline_t;

static renderPipeline_t _STLShaderPipeline;
static unsigned _VAOs[1];
static unsigned _VBOs[1];
static uint32_t _numTriangles;
static float _scale;

static renderPipeline_t initSTLPipeline(const char *stlFilename, const char *shaderPath, unsigned VAO, unsigned VBO)
{
    renderPipeline_t pipeline;
    pipeline.VAO = VAO;
    pipeline.VBO = VBO;

    // Get shader file paths from shader path.
	char vertexSourceFile[64];
    char geoSourceFile[64];
	char fragSourceFile[64];
	strcpy(vertexSourceFile, shaderPath);
	strcpy(geoSourceFile, shaderPath);
	strcpy(fragSourceFile, shaderPath);
	strcat(vertexSourceFile, "/basic.vs");
	strcat(geoSourceFile, "/normals.gs");
	strcat(fragSourceFile, "/basic.fs");

	unsigned vertexShader = abstrShaderConstruct(GL_VERTEX_SHADER, vertexSourceFile);
    unsigned geometryShader = abstrShaderConstruct(GL_GEOMETRY_SHADER, geoSourceFile);
	unsigned fragmentShader = abstrShaderConstruct(GL_FRAGMENT_SHADER, fragSourceFile);

	pipeline.program = abstrShaderProgramConstruct(
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

	glBindVertexArray(VAO);

    // Allocate and copy vertices to VBO.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, stl.polyCount*9*sizeof(float), stl.vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
    free(stl.vertices);

    // Unbind VAO.
	glBindVertexArray(0); 

    return pipeline;
}

void renderInit(const char *stlFilename, const char *shaderPath)
{
	glGenVertexArrays(sizeof(_VAOs)/sizeof(_VAOs[0]), _VAOs);
	glGenBuffers(sizeof(_VBOs)/sizeof(_VBOs[0]), _VBOs);

    _STLShaderPipeline = initSTLPipeline(stlFilename, shaderPath, _VAOs[0], _VBOs[0]);

    // Setup openGL settings.
	glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
}

static void drawSTL(mat4 modelMat, mat4 viewMat, mat4 projectionMat)
{
    glUseProgram(_STLShaderPipeline.program);
    glBindVertexArray(_STLShaderPipeline.VAO);

    abstrShaderSetUniformMatrix4fv(_STLShaderPipeline.program, "model", (float*)modelMat);
    abstrShaderSetUniformMatrix4fv(_STLShaderPipeline.program, "view", (float*)viewMat);
    abstrShaderSetUniformMatrix4fv(_STLShaderPipeline.program, "projection", (float*)projectionMat);
    abstrShaderSetUniform3fv(_STLShaderPipeline.program, "lightPos", (float*)lightPos);

	glDrawArrays(GL_TRIANGLES, 0, 3*_numTriangles);

	glBindVertexArray(0);
}

void renderLoop(float aspectRatio, float horzDeltaRad, float vertDeltaRad, float deltaScale)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static float horzTheta = 0.0f;
	static float vertTheta = 0.0f;
    static float localScale = 1.0f;
	horzTheta += horzDeltaRad;
	vertTheta += vertDeltaRad;
    localScale += deltaScale;

	mat4 modelMat;
	glm_mat4_identity(modelMat);
    glm_translate(modelMat, modelPos);
	glm_rotate_x(modelMat, vertTheta, modelMat);
	glm_rotate_y(modelMat, horzTheta, modelMat);
	glm_scale_uni(modelMat, _scale * localScale);

    mat4 viewMat;
    glm_mat4_identity(viewMat);
    glm_lookat(cameraPos, cameraBoresight, GLM_YUP, viewMat);

	mat4 projectionMat;
	glm_perspective(glm_rad(CAMERA_FOV_DEG), aspectRatio, 0.1f, 100.0f, projectionMat);

    drawSTL(modelMat, viewMat, projectionMat);
}

void renderTerminate(void)
{
	glDeleteVertexArrays(sizeof(_VAOs)/sizeof(_VAOs[0]), _VAOs);
	glDeleteBuffers(sizeof(_VBOs)/sizeof(_VBOs[0]), _VBOs);
	glDeleteProgram(_STLShaderPipeline.program);

	glfwTerminate();
}
