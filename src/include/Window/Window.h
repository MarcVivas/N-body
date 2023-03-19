//
// Created by marc on 18/03/23.
//

#ifndef N_BODY_WINDOW_H
#define N_BODY_WINDOW_H


class Window {
public:
    Window(glm::vec2 window, const char * title);
    void updateWindowSize(const int width, const int height);
    unsigned int getWindowWidth() const;
    unsigned int getWindowHeight() const;
    GLFWwindow* getWindow();
    void updateWindowTitle(const char * title);

private:
    GLFWwindow* window;
    std::string windowTitle;
    unsigned int windowWidth, windowHeight;

    void initGlfw();
    void initGlad();
    void createWindow();
};


#endif //N_BODY_WINDOW_H
