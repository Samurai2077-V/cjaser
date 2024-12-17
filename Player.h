#pragma once
#include "Object.h"
#include "Camera.h"

class GunModel {
public:
    GEMModelanimation Animation;
    Vec3 offsetPosition{ 0.0f, 10.0f, 0.0f };   
    Vec3 soldierscale{ 1.0f, 1.0f, 1.0f };     
    void init(DXcore& core, const std::string& filename, TextureManager& textures) {
        Animation.init(core, filename, textures);
    }
    void update(const Vec3& newPosition, const Vec3& newTarget, float dt) {
        Animation.setPosition(newPosition);
        Animation.looktarget(newTarget);
    }
    void draw(shader* shader, DXcore& core, TextureManager& textures, float dt, const CameraManager& cameraManager) {
        Animation.setcollider(Vec3(0.0f, 0.0f, 0.0f));
        update(cameraManager.camera.position, cameraManager.camera.target, dt);
        const bool leftMouse = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        const bool rightMouse = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
        if (leftMouse && rightMouse) {
            Animation.ani("Armature|13 Zoom Fire", dt);
        }
        else if (rightMouse) {
            Animation.ani("Armature|11 Zoom Idle", dt);
        }
        else if (leftMouse) {
            Animation.ani("Armature|08 Fire", dt);
        }
        else {
            Animation.ani("Armature|04 Idle", dt);
        }
        Animation.draw(shader, core, textures, dt, cameraManager);
    }
    void scale(const Vec3& scale) {
        Animation.scaleModel(scale);
    }
    void rotationX(float degree) {
        Animation.rotationX(degree);
    }
};

