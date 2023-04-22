
#include "Window.h"
#include <iostream>

Window::Window(glm::vec2 window, const char * title): windowWidth(window.x), windowHeight(window.y), windowTitle(std::string(title).append(" ")){
    this->initGlfw();
    this->createWindow();
    this->initGlad();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
}

void Window::initGlfw(){
    glfwInit();
    // OpenGL 4.5
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void Window::initGlad(){
    // Load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
}

void Window::createWindow() {
    this->window = glfwCreateWindow(this->windowWidth, this->windowHeight, this->windowTitle.c_str(), nullptr, nullptr);

    if (this->window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    // Tell GLFW to make the context of our window the main context on the current thread
    glfwMakeContextCurrent(this->window);

    // Tell GLFW to capture our mouse
    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}


GLFWwindow *Window::getWindow() {
    return this->window;
}

void Window::updateWindowSize(const int width, const int height) {
    this->windowHeight = height;
    this->windowWidth = width;
    // The first two parameters of glViewport set the location of the lower left corner of the window.
    glViewport(0, 0, width, height);
}

void Window::updateWindowTitle(const char *title) {
    glfwSetWindowTitle(this->getWindow(), title);
}

Window::Window() = default;