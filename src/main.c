#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h> 

#include <internal/abstrShader.h>
#include <internal/render.h>

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define INPUT_ROTATION_SPEED_RPS	(35.0f)
#define AUTO_ROTATION_PERIOD_S		(4.0f)
#define DEBOUNCE_THRESHOLD_MS		(200.0f)
#define MOUSE_SENS                  (0.75f)

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main(int argc, char **argv)
{
	// Handle Arguments
	if (argc != 3) {
		printf("Usage %s <Target STL> <Shader Directory>\n", argv[0]);
		return -1;
	}
	char *stl_filename = argv[1];
	char *shader_dir = argv[2];

	// ------------------------------
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// --------------------
	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "fstl-view", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSwapInterval(1); // enable V-Sync.

	// ---------------------------------------
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD\n");
		return -1;
	}

	char vertexSourceFile[64];
    char geoSourceFile[64];
	char fragSourceFile[64];
	strcpy(vertexSourceFile, shader_dir);
	strcpy(geoSourceFile, shader_dir);
	strcpy(fragSourceFile, shader_dir);
	strcat(vertexSourceFile, "/basic.vs");
	strcat(geoSourceFile, "/normals.gs");
	strcat(fragSourceFile, "/basic.fs");
	renderInit(stl_filename, vertexSourceFile, geoSourceFile, fragSourceFile);

	float autospinToggleDbTime = 0.0f;
	float wireframeToggleDbTime = 0.0f;
	float prevTimeValue = 0.0f;
	bool useWireframe = true;
    bool useAutospin = false;
    bool useManualRotation = false;

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		int width, height;
		glfwGetWindowSize(window, &width, &height);
		float aspectRatio = (float)width / (float)height;

        static double prevMouseX = 0;
        static double prevMouseY = 0;
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        double deltaMouseX = mouseX - prevMouseX;
        double deltaMouseY = mouseY - prevMouseY;
        prevMouseX = mouseX;
        prevMouseY = mouseY;

        bool mousePress = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        if (mousePress != useManualRotation) {
            if (mousePress)
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            else
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        useManualRotation = mousePress;

		float timeValue = glfwGetTime();
		float deltaTime = timeValue - prevTimeValue;
		prevTimeValue = timeValue;

		wireframeToggleDbTime += deltaTime;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS 
				&& wireframeToggleDbTime > DEBOUNCE_THRESHOLD_MS / 1000.0f) {
			wireframeToggleDbTime = 0.0f;

			useWireframe = !useWireframe;
			if (useWireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glCullFace(GL_BACK);
				glEnable(GL_CULL_FACE);
			}
		}

		autospinToggleDbTime += deltaTime;
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS 
				&& autospinToggleDbTime > DEBOUNCE_THRESHOLD_MS / 1000.0f) {
			useAutospin = !useAutospin;
			autospinToggleDbTime = 0.0f;
		}

		// Handle vertical rotation amount.
		float vertDeltaRad = 0.0f;
		if (useManualRotation)
            vertDeltaRad = MOUSE_SENS * deltaMouseY * deltaTime;

		// Handle horizontal rotation amount.
		float horzDeltaRad = 0.0f;
		if (useManualRotation) {
            horzDeltaRad = MOUSE_SENS * deltaMouseX * deltaTime;
		}
		else if (useAutospin) {
			horzDeltaRad = 2.0f * M_PI * (1.0f/AUTO_ROTATION_PERIOD_S) * deltaTime;
		}

		renderLoop(aspectRatio, horzDeltaRad, vertDeltaRad);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	renderTerminate();

	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
static void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
