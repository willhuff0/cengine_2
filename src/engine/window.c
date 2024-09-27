//
// Created by wehuf on 9/26/2024.
//

#include "window.h"

#include "input.h"

#define DEFAULT_WINDOW_WIDTH 1920
#define DEFAULT_WINDOW_HEIGHT 1080

static GLFWwindow* window;
static int windowWidth, windowHeight;

static void error(const char* message) {
    fprintf(stderr, "[ENGINE.WINDOW]: %s\n", message);
}

static void glfw_ErrorCallback(int error, const char* description) {
    fprintf(stderr, "[ENGINE.WINDOW.GLFW (%d)]: %s\n", error, description);
}

static void glfw_FramebufferResizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, windowWidth = width, windowHeight = height);
}


void initWindow() {
    window = NULL;
    windowWidth = DEFAULT_WINDOW_WIDTH;
    windowHeight = DEFAULT_WINDOW_HEIGHT;

    glfwSetErrorCallback(glfw_ErrorCallback);

    glfwInitHint(GLFW_ANGLE_PLATFORM_TYPE, GLFW_ANGLE_PLATFORM_TYPE_D3D11);
    if (!glfwInit()) {
        error("Failed to initialize GLFW");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_SAMPLES, 8);
    window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "cengine_2", NULL, NULL);
    if (window == NULL) {
        error("Failed to create window.");
        glfwTerminate();
        return;
    }

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, mode->width / 2.0f - DEFAULT_WINDOW_WIDTH / 2.0f, mode->height / 2.0f - DEFAULT_WINDOW_HEIGHT / 2.0f);

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, glfw_FramebufferResizeCallback);
}
void freeWindow() {
    glfwSetErrorCallback(NULL);

    if (window != NULL) {
        glfwSetFramebufferSizeCallback(window, NULL);
        glfwDestroyWindow(window);
        window = NULL;
    }

    glfwTerminate();
}


bool windowShouldClose() {
    return glfwWindowShouldClose(window);
}

void windowPollEvents() {
    glfwPollEvents();
}

void windowSwapBuffers() {
    glfwSwapBuffers(window);
}

void windowHideCursor() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}
void windowShowCursor() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void windowEnableInputCallbacks() {
    glfwSetKeyCallback(window, inputKeyCallback);
    glfwSetMouseButtonCallback(window, inputMouseButtonCallback);
    glfwSetCursorPosCallback(window, inputMousePosCallback);
}
void windowDisableInputCallbacks() {
    glfwSetKeyCallback(window, NULL);
    glfwSetMouseButtonCallback(window, NULL);
    glfwSetCursorPosCallback(window, NULL);
}

const char* windowGetGLFWVersionString() {
    return glfwGetVersionString();
}