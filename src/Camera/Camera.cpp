#include "Camera.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>

//--------------------------------------------------
// Constructor
//--------------------------------------------------
Camera::Camera(glm::vec2 window, glm::vec3 worldDim) {
    // Set the center of the world (focal point)
    worldCenter = glm::vec3(worldDim.x / 2.0f, worldDim.y / 2.0f, worldDim.z / 2.0f);
    targetWorldCenter = worldCenter;

    // Initialize spherical coordinates (in radians)
    yaw = glm::radians(45.0f);
    pitch = glm::radians(30.0f);
    distance = glm::length(worldDim) * 0.75f;

    // Set target values equal to current values
    targetYaw = yaw;
    targetPitch = pitch;
    targetDistance = distance;

    updateCameraPosition();

    // Up vector remains constant
    up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Camera parameters
    sensitivity = .001f;
    zoomSensitivity = 0.1f;
    fov = 45.0f;
    aspectRatio = window.x / window.y;
    nearClipPlane = distance * 0.01f;
    farClipPlane = distance * 10.0f;

    // Damping factors
    rotationDamping = 10.0f;  // radians per second
    zoomDamping = 5.0f;
    panDamping = 10.0f;

    // Input initialization
    isDragging = false;
    prevMousePos = glm::vec2(0.0f);
}

//--------------------------------------------------
// Helper: Update camera position based on spherical coordinates and worldCenter
//--------------------------------------------------
void Camera::updateCameraPosition() {
    position.x = worldCenter.x + distance * cos(pitch) * sin(yaw);
    position.y = worldCenter.y + distance * sin(pitch);
    position.z = worldCenter.z + distance * cos(pitch) * cos(yaw);
}

//--------------------------------------------------
// Setters / Getters
//--------------------------------------------------
void Camera::setAspectRatio(glm::vec2 window) {
    aspectRatio = window.x / window.y;
}

glm::vec3 Camera::getPosition() const {
    return position;
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, worldCenter, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspectRatio, nearClipPlane, farClipPlane);
}

glm::mat4 Camera::getModelMatrix() const {
    return glm::mat4(1.0f);
}

glm::mat4 Camera::getModelViewProjection() const {
    return getProjectionMatrix() * getViewMatrix() * getModelMatrix();
}

void Camera::setIsDragging(bool dragging) {
    isDragging = dragging;
}

void Camera::setPreviousMousePos(glm::vec2 prevPos) {
    prevMousePos = prevPos;
}

//--------------------------------------------------
// Input Callbacks
//--------------------------------------------------

// Rotate based on mouse movement. This modifies the target yaw and pitch.
void Camera::rotateCallback(glm::vec2 mousePos) {
    if (!isDragging) {
        return;
    }
    glm::vec2 delta = mousePos - prevMousePos;
    prevMousePos = mousePos;

    // Adjust target values using sensitivity scaled by current distance (for dynamic control)
    targetYaw += -delta.x * sensitivity * distance;
    targetPitch += delta.y * sensitivity * distance;

    // Clamp pitch to avoid flipping
    float pitchLimit = glm::radians(89.0f);
    if (targetPitch > pitchLimit)
        targetPitch = pitchLimit;
    if (targetPitch < -pitchLimit)
        targetPitch = -pitchLimit;
}

// Zoom callback adjusts the target distance (exponentially)
void Camera::zoomCallback(float mouseWheelYOffset) {
    targetDistance *= (1.0f - mouseWheelYOffset * zoomSensitivity);
    // Clamp the target distance
    const float minDistance = 1.0f;
    const float maxDistance = 10000.0f;
    if (targetDistance < minDistance)
        targetDistance = minDistance;
    if (targetDistance > maxDistance)
        targetDistance = maxDistance;
}

// Pan callback moves the target world center based on mouse movement.
// You might use a modifier key or a different mouse button for panning.
void Camera::panCallback(glm::vec2 mouseDelta) {
    // Calculate right and up directions based on current camera orientation
    glm::vec3 direction = glm::normalize(worldCenter - position);
    glm::vec3 right = glm::normalize(glm::cross(direction, up));
    glm::vec3 cameraUp = glm::normalize(glm::cross(right, direction));

    // Adjust target center based on the mouse delta scaled by distance and pan damping factor
    targetWorldCenter += (-right * mouseDelta.x + cameraUp * mouseDelta.y) * (distance * sensitivity);
}

//--------------------------------------------------
// Update the camera smoothly. Call this each frame with the elapsed time.
//--------------------------------------------------
void Camera::update(float dt) {
    // Smoothly interpolate rotation and zoom using damping factors
    float factorRot = 1.0f - expf(-rotationDamping * dt);
    float factorZoom = 1.0f - expf(-zoomDamping * dt);
    float factorPan = 1.0f - expf(-panDamping * dt);

    yaw += (targetYaw - yaw) * factorRot;
    pitch += (targetPitch - pitch) * factorRot;
    distance += (targetDistance - distance) * factorZoom;
    worldCenter += (targetWorldCenter - worldCenter) * factorPan;

    // Update clip planes based on new distance
    nearClipPlane = distance * 0.01f;
    farClipPlane = distance * 10.0f;

    updateCameraPosition();
}