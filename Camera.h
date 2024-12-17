#pragma once
#include <windows.h>
#include "Math.h"
#include "DXcore.h"
#include <cmath>

class Camera {
public:
    Vec3 position;
    Vec3 target;
    Vec3 up;
    float yaw;
    float pitch;
    float fov;
    float height;
    float width;
    float nearPlane;
    float farPlane;
    Matrix viewMatrix;
    Matrix projectionMatrix;

private:
    const float Pi = 3.14f;
public:
    Camera()
        : position(Vec3(0.0f, 10.0f, 0.0f)),
        target(Vec3(0.0f, 0.0f, 0.0f)),
        up(Vec3(0.0f, 1.0f, 0.0f)),
        yaw(90.0f),
        pitch(0.0f),
        fov(45.0f),
        height(9.0f),
        width(16.0f),
        nearPlane(0.1f),
        farPlane(1000.0f) {
        updateProjectionMatrix();
        updateViewMatrix();
    }

    void setPosition(const Vec3& newPosition) {
        position = newPosition;
        updateViewMatrix();
    }
    Vec3 getTarget() const {
        return target;
    }
    void setTarget(const Vec3& newTarget) {
        target = newTarget;
        updateViewMatrix();
    }
    void setProjection(float newFov, float newHeight, float newWidth, float newNear, float newFar) {
        fov = newFov;
        height = newHeight;
        width = newWidth;
        nearPlane = newNear;
        farPlane = newFar;
        updateProjectionMatrix();
    }

    void updateViewMatrix() {
        viewMatrix = viewMatrix.lookat(position, target, up);
    }
    void updateProjectionMatrix() {
        projectionMatrix = projectionMatrix.PerPro(height, width, fov, farPlane, nearPlane);
    }

    void processMouseInput(float deltaX, float deltaY, float sensitivity) {
        yaw += deltaX * sensitivity;
        pitch -= deltaY * sensitivity;
        pitch = (pitch < -89.0f) ? -89.0f : (pitch > 89.0f ? 89.0f : pitch);
        updateCameraDirection();
    }

    void updateCameraDirection() {
        float radYaw = yaw * (Pi / 180.0f);
        float radPitch = pitch * (Pi / 180.0f);
        Vec3 direction;
        direction.x = cos(radYaw) * cos(radPitch);
        direction.y = sin(radPitch);
        direction.z = sin(radYaw) * cos(radPitch);
        target = position + direction.normalize();
        updateViewMatrix();
    }
    void move(const Vec3& direction, float speed, float dt) {
        Vec3 movement = direction * speed * dt;
        position += movement;
        target += movement;
        updateViewMatrix();
    }
    void moveForward(float speed, float dt) {
        Vec3 forward = (target - position).normalize();
        forward.y = 0.0f; 
        move(forward, speed, dt);
    }

    void moveBackward(float speed, float dt) {
        Vec3 backward = (position - target).normalize();
        backward.y = 0.0f;
        move(backward, speed, dt);
    }
    void moveRight(float speed, float dt) {
        Vec3 forward = (target - position).normalize();
        Vec3 right = forward.Cross(up).normalize();
        move(right, speed, dt);
    }
    void moveLeft(float speed, float dt) {
        Vec3 forward = (target - position).normalize();
        Vec3 left = up.Cross(forward).normalize();
        move(left, speed, dt);
    }
    Vec3 getPosition() const {
        return position;
    }
};

class CameraManager {
public:
    Camera camera;
    Matrix VP;
    void lockMouseToScreenCenter(HWND hwnd, float sensitivity) {
        POINT center;
        RECT rect;
        GetClientRect(hwnd, &rect);
        center.x = (rect.right - rect.left) / 2;
        center.y = (rect.bottom - rect.top) / 2;
        static POINT lastCursorPos = { center.x, center.y };
        POINT currentPos;
        GetCursorPos(&currentPos);
        ScreenToClient(hwnd, &currentPos);
        float deltaX = static_cast<float>(center.x - currentPos.x);
        float deltaY = static_cast<float>(currentPos.y - center.y);
        camera.processMouseInput(deltaX, deltaY, sensitivity);
        ClientToScreen(hwnd, &center);
        SetCursorPos(center.x, center.y);
        lastCursorPos = currentPos;
    }
    void initialize(const DXcore& core, const TextureManager& textures) {
        camera.setPosition(Vec3(0.0f, 10.0f, 0.0f));
        camera.setTarget(Vec3(0.0f, 0.0f, 0.0f));
        camera.setProjection(45.0f, 9.0f, 16.0f, 0.1f, 1000.0f);
    }
    void initialize(const Vec3& position, const Vec3& target, float fov, float height, float width, float nearPlane, float farPlane) {
        camera.setPosition(position);
        camera.setTarget(target);
        camera.setProjection(fov, height, width, nearPlane, farPlane);
    }
    void handleInput(HWND hwnd, float sensitivity, float dt, const bool keys[]) {
        lockMouseToScreenCenter(hwnd, sensitivity);
        float movementSpeed = 50.0f;
        if (keys['W']) camera.moveForward(movementSpeed, dt);
        if (keys['S']) camera.moveBackward(movementSpeed, dt);
        if (keys['D']) camera.moveRight(movementSpeed, dt);
        if (keys['A']) camera.moveLeft(movementSpeed, dt);
        VP = camera.viewMatrix * camera.projectionMatrix;
    }
};
