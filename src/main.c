#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h> 

#include <internal/abstrShader.h>
#include <internal/render.h>
#include <internal/input.h>

#include <stdio.h>
#include <string.h>

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

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

    // Initialize OpenGL viewport.
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	renderInit(stl_filename, shader_dir);

    inputInit(window);

    float prevTimeValue = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		float timeValue = glfwGetTime();
		float deltaTime = timeValue - prevTimeValue;
		prevTimeValue = timeValue;

        float xDeltaRad, yDeltaRad, deltaScaleNorm;
        inputLoop(window, deltaTime, &xDeltaRad, &yDeltaRad, &deltaScaleNorm);

		int width, height;
		glfwGetWindowSize(window, &width, &height);
		float aspectRatio = (float)width / (float)height;

		renderLoop(aspectRatio, xDeltaRad, yDeltaRad, deltaScaleNorm);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	renderTerminate();

	return 0;
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}
