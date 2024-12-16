#pragma once
#include <windows.h>
#include "DXcore.h"
#include <cmath>
#include "Mash.h"


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
    float pi = 3.141592653589793f;
    bool isJumping = false;      
    float jumpVelocity = 0.0f;   
    float gravity = -19.6f;       
    float jumpHeight = 3.0f;     
    float groundY = 10.0f;       


    

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

    Vec3 front() const {
        return (target - position).normalize(); 
    }


    void setPosition(const Vec3& newPosition) {
        position = newPosition;
        updateViewMatrix();
    }
    
    Vec3 gettarget() {
        return target;
    }
    void setTarget(const Vec3& newTarget) {
        target = newTarget;
        updateViewMatrix();
    }

    void setProjection(float newFov, float newHeight, float newWidth, float newNear, float newFar) {
        fov = newFov;
        height = newHeight;
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

    void processMouseInput(float mouseX, float mouseY, float sensitivity) {
        yaw += mouseX * sensitivity;
        pitch -= mouseY * sensitivity;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        updateCameraDirection();
    }

    void updateCameraDirection() {
        float radYaw = yaw * (pi / 180.0f);
        float radPitch = pitch * (pi / 180.0f);

        Vec3 direction;
        direction.x = cos(radYaw) * cos(radPitch);
        direction.y = sin(radPitch);
        direction.z = sin(radYaw) * cos(radPitch);

        target = position + direction.normalize();
        updateViewMatrix();
    }

    void moveForward(float speed, float dt) {
        Vec3 forward = (target - position).normalize();

        
        forward.y = 0.0f;
        forward = forward.normalize();

        position += forward * speed * dt;
        target += forward * speed * dt;
        updateViewMatrix();
    }

    void moveBackward(float speed, float dt) {
        Vec3 forward = (target - position).normalize();

        
        forward.y = 0.0f;
        forward = forward.normalize();

        position -= forward * speed * dt;
        target -= forward * speed * dt;
        updateViewMatrix();
    }

    void moveRight(float speed, float dt) {
        Vec3 forward = (target - position).normalize();
        Vec3 right = forward.Cross(up).normalize();
        position += right * speed * dt;
        target += right * speed * dt;
        updateViewMatrix();
    }

    void moveLeft(float speed, float dt) {
        Vec3 forward = (target - position).normalize();
        Vec3 right = forward.Cross(up).normalize();
        position -= right * speed * dt;
        target -= right * speed * dt;
        updateViewMatrix();
    }

    void moveUp(float speed, float dt) {
        if (speed > 0.0f && !isJumping) {
            isJumping = true;
            jumpVelocity = sqrt(2.0f * abs(gravity) * jumpHeight);  
        }

        if (isJumping) {
           
            position.y += jumpVelocity * dt;
            target.y += jumpVelocity * dt;

            
            jumpVelocity += gravity * dt;

            
            if (position.y <= groundY) {
                position.y = groundY;
                target.y = groundY;
                isJumping = false; 
            }
            updateViewMatrix();
        }
    }



    Vec3 getWorldPosition() const {
        return position;
    }
    Vec3 getPosition() {
        return position;
    }

};

class CameraManager {
public:
    Camera camera;
    Matrix vp;
    Vec3 front = (camera.target - camera.position).normalize();

    

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

    void initialize(DXcore& core, TextureManager& textures) {
        camera = Camera();
        
    }

    void initialize(Vec3& position, Vec3& target, float fov, float height, float width, float nearPlane, float farPlane) {
        camera.setPosition(position);
        camera.setTarget(target);
        camera.setProjection(height, width, fov, farPlane, nearPlane);
    }

    void handleInput(HWND hwnd, float sensitivity, float dt, bool keys[]) {
        lockMouseToScreenCenter(hwnd, sensitivity);
        camera.moveForward(keys['W'] ? 10.0f : 0.0f, dt);
        camera.moveBackward(keys['S'] ? 10.0f : 0.0f, dt);
        camera.moveRight(keys['D'] ? 10.0f : 0.0f, dt);
        camera.moveLeft(keys['A'] ? 10.0f : 0.0f, dt);
        camera.moveUp(GetAsyncKeyState(VK_SPACE) & 0x8000 ? 10.0f : 0.0f, dt);  


        vp = camera.viewMatrix * camera.projectionMatrix;
        
       
    }
    

    
};