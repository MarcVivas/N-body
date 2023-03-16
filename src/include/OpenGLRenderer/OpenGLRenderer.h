//
// Created by marc on 4/03/23.
//

#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H


class OpenGLRenderer {

public:

    // Constructor
    OpenGLRenderer(unsigned int window_width, unsigned int window_height, const char * title, bool VSync, bool showFPS);
    OpenGLRenderer(Camera *camera, const char * title, bool VSync, bool showFPS);

    // Destructor
    ~OpenGLRenderer();

    // Render loop
    void render_loop(ParticleSystem* particleSystem);

    // Getters and setters

    void setDeltaTime(double time);

    double getDeltaTime();



private:
    GLFWwindow* window;
    std::string window_title;
    int WINDOW_HEIGHT;
    int WINDOW_WIDTH;
    double deltaTime;
    bool showFPS;
    bool pauseSimulation;
    Camera *camera;

    void init_glfw();

    void init_glad();

    void createWindow(unsigned int window_width, unsigned int window_height, const char * title);

    void updateFPS(double &previousTime, size_t &frameCount);

    void updateDeltaTime(double &lastFrameTime);

    void setKeyCallback();

    void setMouseButtonCallback();

    // Resizeable window
    void setFramebufferSizeCallback();

    // Cursor movement
    void setCursorPosCallback();

    // Mouse scroll callback
    void setScrollCallback();

    bool getShowFPS();

    GLFWwindow* getWindow();

};


#endif //OPENGLRENDERER_H
