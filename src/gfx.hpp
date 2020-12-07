#pragma once


#include <GLFW/glfw3.h>

#include <iostream>

// //////////////////////////////////////////////////////////////////
// Window setup
void glInfo() {
    using std::cout;
    using std::endl;
    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

void init() {
    if (!glfwInit()) {
        std::cout << "Can't init GLFW\n";
        exit(1);
    }
}

GLFWwindow* initWindow(int height, int width, std::string title) {
    // @TODO: this doesn't work on WSL2 on Windows 10
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr) {
        const char* description;
        int code = glfwGetError(&description);
        std::cout << "Can't create window : " << code << " : " << std::string(description) << "\n";
        exit(2);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Setup 2D rendering
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glEnable(GL_MULTISAMPLE);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    return window;
}


void renderGrid(int width, int height, int size) {
    // Render grid
    glColor3f(0.1, 0.1, 0.1);
    glBegin(GL_LINES);
    for (int i = 0; i < width; i += size) {
        glVertex2f(i, 0);
        glVertex2f(i, height);
    }
    for (int j = 0; j < height; j += size) {
        glVertex2f(0, j);
        glVertex2f(width, j);
    }
    glEnd();
}