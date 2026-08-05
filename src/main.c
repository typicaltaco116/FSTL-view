#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <internal/abstrShader.h>
#include <internal/render.h>

#include <stdio.h>
#include <string.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main(int argc, char **argv)
{
	// Handle Arguments
	if (argc != 2) {
		printf("Usage %s <Shader Directory>\n", argv[0]);
		return -1;
	}
	char *shader_dir = argv[1];

	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to initialize GLAD\n");
		return -1;
	}

	char vertexSourceFile[64];
	char fragSourceFile[64];
	strcpy(vertexSourceFile, shader_dir);
	strcpy(fragSourceFile, shader_dir);
	strcat(vertexSourceFile, "/basicTransform.vert");
	strcat(fragSourceFile, "/basic.frag");
	renderInit(vertexSourceFile, fragSourceFile);

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		int width, height;
		glfwGetWindowSize(window, &width, &height);
		float aspectRatio = (float)width / (float)height;
		renderLoop(aspectRatio);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	renderTerminate();

	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


