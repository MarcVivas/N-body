#ifndef N_BODY_CAMERA_H
#define N_BODY_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    // Constructor: initializes camera using window size and world dimensions
    Camera(glm::vec2 window, glm::vec3 worldDim);

    // Update the aspect ratio (e.g. on window resize)
    void setAspectRatio(glm::vec2 window);

    // Get combined Model-View-Projection matrix
    glm::mat4 getModelViewProjection() const;

    // Input callbacks
    void rotateCallback(glm::vec2 mousePos);
    void zoomCallback(float mouseWheelYOffset);
    void panCallback(glm::vec2 mouseDelta);

    // Updates the camera state; call every frame with elapsed time (dt)
    void update(float dt);

    void setIsDragging(bool dragging);
    void setPreviousMousePos(glm::vec2 prevPos);
    glm::vec3 getPosition() const;

    // Get individual matrices
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getModelMatrix() const;

private:
    // Internal helper to update the camera position from spherical coordinates.
    void updateCameraPosition();

    // The focal point that the camera orbits
    glm::vec3 worldCenter;
    glm::vec3 position;   // Current camera position
    glm::vec3 up;         // Up direction

    // Spherical coordinates for the orbit camera (current state)
    float yaw;
    float pitch;
    float distance;

    // Target values for smooth damping
    float targetYaw;
    float targetPitch;
    float targetDistance;
    glm::vec3 targetWorldCenter;

    // Camera parameters
    float sensitivity;
    float zoomSensitivity;
    float fov;            // Field-of-view in degrees
    float aspectRatio;
    float nearClipPlane;
    float farClipPlane;

    // Damping factors (the higher, the snappier the camera)
    float rotationDamping;
    float zoomDamping;
    float panDamping;

    // Input state
    bool isDragging;
    glm::vec2 prevMousePos;
};

#endif //N_BODY_CAMERA_H