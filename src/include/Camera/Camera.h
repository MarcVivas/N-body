//
// This code is not 100% mine, although is modified by me (marc) on 15/03/2023
// The code was written with the help of ChatGPT
//

#ifndef N_BODY_CAMERA_H
#define N_BODY_CAMERA_H

class Camera {
public:
    // Constructor, sets default values for camera properties
    Camera(glm::vec2 window, glm::vec3 worldDim);

    // Set aspect ratio
    void setAspectRatio(unsigned int winWidth, unsigned int winHeight);

    // Get the model view projection matrix
    glm::mat4 getModelViewProjection() const;

    // Zoom the camera in/out
    void zoomCallback(float mouseWheelYOffset);

    // Rotate the camera based on mouse movement
    void rotateCallback(glm::vec2 mousePos);

    void setIsDragging(bool dragging);

    void setPreviousMousePos(glm::vec2 prevPos);

private:
    glm::vec3 worldDimensions;
    glm::vec3 position;
    glm::vec3 worldCenter;
    glm::vec3 up;
    float sensitivity;
    float fov;
    float aspectRatio;
    float nearClipPlane;
    float farClipPlane;
    bool isDragging;     // whether the mouse is dragging the camera
    glm::vec2 prevMousePos;     // the previous mouse position

    // Get the view matrix
    glm::mat4 getViewMatrix() const;

    // Get the projection matrix
    glm::mat4 getProjectionMatrix() const;

    // Get the model matrix
    glm::mat4 getModelMatrix() const;

};

#endif //N_BODY_CAMERA_H
