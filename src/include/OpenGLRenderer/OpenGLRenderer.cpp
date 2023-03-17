//
// Created by marc on 4/03/23.
//
#include "OpenGLRenderer.h"

/**
 * Constructor
 * @param window_width
 * @param window_height
 * @param title
 * @param VSync
 * @param showFramesPerSecond
 */
OpenGLRenderer::OpenGLRenderer(unsigned int window_width, unsigned int window_height, const char *title, bool VSync, bool showFramesPerSecond) : showFPS(showFramesPerSecond), pauseSimulation(true) {
    this->init_glfw();
    this->createWindow(window_width, window_height, title);
    this->init_glad();
    glEnable(GL_DEPTH_TEST);
    this->setFramebufferSizeCallback();
    this->setKeyCallback();
    if(!VSync){
        // Disable Vsync
        glfwSwapInterval(0);
    }
}


/**
 * 3D renderer constructor
 * @param camera
 * @param title
 * @param VSync
 * @param showFPS
 */
OpenGLRenderer::OpenGLRenderer(Camera *cam, const char *title, bool VSync, bool showFramesPerSecond): camera(cam), showFPS(showFramesPerSecond), pauseSimulation(true) {
    this->camera = cam;
    this->init_glfw();
    this->createWindow(this->camera->getWindowWidth(), this->camera->getWindowHeight(), title);
    this->init_glad();
    glEnable(GL_DEPTH_TEST);
    this->setFramebufferSizeCallback();
    this->setKeyCallback();
    this->setScrollCallback();
    if(!VSync){
        // Disable Vsync
        glfwSwapInterval(0);
    }
}


/**
 * Destructor
 */
OpenGLRenderer::~OpenGLRenderer(){
    delete this->camera;
    // Terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
}

/**
 *
 * @tparam Function
 * @param display
 */
void OpenGLRenderer::render_loop(ParticleSystem* particleSystem) {

    // Render stats variables
    // ========================
    double previousTime = glfwGetTime();
    size_t frameCount = 0;
    // ========================

    // Variable used to compute delta time (time between 2 frames)
    double lastFrameTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        this->updateFPS(previousTime, frameCount);

        // Compute time between 2 frames
        this->updateDeltaTime(lastFrameTime);

        if(!this->pauseSimulation){
            particleSystem->update(this->getDeltaTime());
        }

        particleSystem->draw(this->camera);

        // ======================
        // Swap buffers: Front buffer(render) and back buffer (next render)
        glfwSwapBuffers(window);
        // ======================

        // ======================
        // Checks if any events are triggered (keys pressed/released, mouse moved etc.) and calls the corresponding functions
        glfwPollEvents();
        // ======================

    }


}


/**
 * Compute time (in seconds) between 2 frames
 * @param lastFrameTime
 */
void OpenGLRenderer::updateDeltaTime(double &lastFrameTime) {
    double currentTime = glfwGetTime();
    this->setDeltaTime(currentTime - lastFrameTime);
    lastFrameTime = currentTime;
}


/**
 * This function updates the frames per second (FPS) and the average frame time
 * based on the elapsed time since the last update.
 * @param previousTime
 * @param frameCount
 */
void OpenGLRenderer::updateFPS(double &previousTime, size_t &frameCount) {
    double currentTime = glfwGetTime();
    frameCount++;

    double elapsedTime = currentTime - previousTime;

    // If elapsed time is greater than or equal to 1 second
    if (elapsedTime >= 1.0) {

        // Compute the fps and the average frame time
        double fps = frameCount / elapsedTime;
        double ms_per_frame = 1000.0 / fps;

        if(this->getShowFPS()){
            // Update the window title
            std::ostringstream title;
            title << "N-body simulation | " << ms_per_frame << " ms/frame (" << fps << " FPS)";
            glfwSetWindowTitle(this->getWindow(), title.str().c_str());
        }

        // Reset for the next update
        previousTime = currentTime;
        frameCount = 0;
    }
}


void OpenGLRenderer::init_glfw() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
}

void OpenGLRenderer::init_glad() {
    // Load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
}

void OpenGLRenderer::createWindow(unsigned int window_width, unsigned int window_height, const char *title) {
    // glfw window creation
    this->window_title = std::string(title).append(" ");

    window = glfwCreateWindow(window_width, window_height, title, nullptr, nullptr);

    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    // Tell GLFW to make the context of our window the main context on the current thread
    glfwMakeContextCurrent(window);

    // Tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void OpenGLRenderer::setMouseButtonCallback() {
    //glfwSetMouseButtonCallback(window, mouse_button_callback);
}

/**
 * Set callback functions for when relevant keys are pressed/released to react accordingly
 */
void OpenGLRenderer::setKeyCallback() {
    // Whenever the window changes in size, GLFW calls this function and fills in the proper arguments for you to process.
    glfwSetWindowUserPointer(this->getWindow(), this);
    glfwSetKeyCallback(this->getWindow(), [](GLFWwindow* window, int key, int scancode, int action, int mods) -> void
    {
        auto renderer = static_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(window)); // retrieve the pointer to the instance
        if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
            glfwSetWindowShouldClose(renderer->getWindow(), true);
        }
        if (key == GLFW_KEY_P && action == GLFW_PRESS){
            renderer->pauseSimulation = !renderer->pauseSimulation;
            usleep(150000);
        }
    }
    );
}

/**
 * Resizeable window callback
 */
void OpenGLRenderer::setFramebufferSizeCallback() {
    // Whenever the window changes in size, GLFW calls this function and fills in the proper arguments for you to process.
    glfwSetWindowUserPointer(this->getWindow(), this);
    glfwSetFramebufferSizeCallback(this->getWindow(), [](GLFWwindow* window, int width, int height)->void
    {
        auto renderer = static_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(window)); // retrieve the pointer to the instance
        renderer->camera->setAspectRatio(width, height); // update the width
        glViewport(0, 0, width, height); // The first two parameters of glViewport set the location of the lower left corner of the window.
    });
}

// Cursor/mouse movement
void OpenGLRenderer::setCursorPosCallback() {
    //glfwSetCursorPosCallback(window, mouse_callback);
}

void OpenGLRenderer::setScrollCallback(){
    glfwSetWindowUserPointer(this->getWindow(), this);
    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)->void
    {
        auto renderer = static_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(window)); // retrieve the pointer to the instance
        renderer->camera->zoomCallback(renderer->getDeltaTime(), yoffset);
    });
}

/**
 * Getter for deltaTime
 * @return deltaTime
 */
double OpenGLRenderer::getDeltaTime() {
    return this->deltaTime;
}

/**
 * Setter for deltaTime
 * @param time
 */
void OpenGLRenderer::setDeltaTime(double time) {
    this->deltaTime = time;
}

/**
 * Getter for showFPS
 * @return
 */
bool OpenGLRenderer::getShowFPS() {
    return this->showFPS;
}

/**
 * Getter for window
 * @return
 */
GLFWwindow* OpenGLRenderer::getWindow() {
    return this->window;
}


