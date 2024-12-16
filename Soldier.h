#pragma once

#include "Geometry.h"
#include "Camera.h"

class SoldierModel {
public:
    GEMModelanimation soldierAnimation;
    Vec3 offsetPosition;   
    Vec3 soldierscale;
    AABB collider;
    


    void init(DXcore& core, const std::string& filename, TextureManager& textures) {
        soldierAnimation.init(core, filename, textures);
        offsetPosition = Vec3(0.0f, 10.0f, 0.0f);  
    }

   
    void update(Vec3 newposition,Vec3 newtarget,float dt, const CameraManager& cameraManager) {
        soldierAnimation.setPosition(newposition);
        soldierAnimation.looktarget(newtarget);
        updateCollider(soldierAnimation.currentPosition, soldierscale);
        
    }
    void updateCollider(const Vec3& position, const Vec3& scale) {
        collider.update(position, scale);
    }

  
    void draw(shader* shader, DXcore& core, TextureManager& textures, float dt, const CameraManager& cameraManager) {
        
        soldierAnimation.setcollider(Vec3(0, 0, 0));
        update(cameraManager.camera.position,cameraManager.camera.target,dt, cameraManager);
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
            soldierAnimation.ani("Armature|13 Zoom Fire", dt);
        }
        else if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
            soldierAnimation.ani("Armature|11 Zoom Idle", dt);
        }
        else if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
            soldierAnimation.ani("Armature|08 Fire", dt);
        }
        else {
            soldierAnimation.ani("Armature|04 Idle", dt);
        }
        
        
        soldierAnimation.draw(shader, core, textures, dt, cameraManager);
    }


    void scale(const Vec3& scale, const CameraManager& cameraManager) {
        soldierAnimation.scaleModel(scale);
        updateCollider(soldierAnimation.currentPosition, scale);
    }
    void rotationX(float degree) {
        soldierAnimation.rotationX(degree);
    }

    bool isColliding(const GEMModelanimation& other) const {
        return collider.isColliding(other.collider);
    }
    
};
