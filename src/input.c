// Primary Header
#include <internal/input.h>

// Standard Headers
#include <stdbool.h>
#include <math.h>

// 3rdParty Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define MOUSE_SENS                  (0.75f)
#define SCROLL_SENS                 (1.0f)
#define INPUT_ROTATION_SPEED_RPS	(35.0f)
#define AUTO_ROTATION_PERIOD_S		(4.0f)
#define DEBOUNCE_THRESHOLD_MS		(100.0f)

static double _prevMouseX = 0.0;
static double _prevMouseY = 0.0;
static bool _useManualRotation = false;
static bool _useWireframe = false;
static float _wireframeDbTime = 0.0f;
static bool _useAutospin = true;
static float _autospinDbTime = 0.0f;
static double _scrollVal;

static bool checkDebouncedToggle(bool input, bool *state, float deltaTime, float *timeTracker);
static void handleToggleInputs(GLFWwindow *window, float deltaTime);
static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void inputInit(GLFWwindow *window)
{
    glfwSetScrollCallback(window, scroll_callback);
}

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    _scrollVal = yoffset;
}

void inputLoop(
        GLFWwindow *window,
        float frameDeltaTime,
        float *horizontalDeltaRad,
        float *verticalDeltaRad,
        float *deltaScaleNorm)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

    handleToggleInputs(window, frameDeltaTime);

    bool mousePress = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    if (mousePress != _useManualRotation) {
        if (mousePress)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    _useManualRotation = mousePress;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    double deltaMouseX = mouseX - _prevMouseX;
    double deltaMouseY = mouseY - _prevMouseY;
    _prevMouseX = mouseX;
    _prevMouseY = mouseY;

    *verticalDeltaRad = 0.0f;
    if (_useManualRotation)
        *verticalDeltaRad = MOUSE_SENS * deltaMouseY * frameDeltaTime;

    *horizontalDeltaRad = 0.0f;
    if (_useManualRotation)
        *horizontalDeltaRad = MOUSE_SENS * deltaMouseX * frameDeltaTime;
    else if (_useAutospin)
        *horizontalDeltaRad = 2.0f * M_PI * (1.0f/AUTO_ROTATION_PERIOD_S) * frameDeltaTime;

    *deltaScaleNorm = SCROLL_SENS * _scrollVal / 15.0f;
    _scrollVal = 0.0;
}

static void handleToggleInputs(GLFWwindow *window, float deltaTime)
{
    bool is_W_pressed = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    if (checkDebouncedToggle(is_W_pressed, &_useWireframe, deltaTime, &_wireframeDbTime)) {
        if (_useWireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_CULL_FACE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_CULL_FACE);
        }
    }

    bool is_R_pressed = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
    checkDebouncedToggle(is_R_pressed, &_useAutospin, deltaTime, &_autospinDbTime);

}

static bool checkDebouncedToggle(bool input, bool *state, float deltaTime, float *timeTracker)
{
    *timeTracker += deltaTime;

    if (!input || *timeTracker < DEBOUNCE_THRESHOLD_MS / 1000.0f)
        return false; // edge not detected.

    *timeTracker = 0.0f;
    *state = !*state;

    return true; // edge detected.
}
