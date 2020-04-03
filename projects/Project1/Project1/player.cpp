#ifndef _CSCI441_PLAYER_
#define _CSCI441_PLAYER_

#include <csci441/matrix4.h>
#include <csci441/vector4.h>
#include <iostream>

#include "camera.h"

class Player {
public:
    Camera camera;
    
    Vector4 position = Vector4();
    Vector4 lookDir = Vector4(0,0,1);
    bool viewAbove = false;

    Player() {
        
        // setup camera
        Matrix4 projection;
        projection.perspective(45, 1, .01, 10);
        camera = Camera();
        camera.projection = projection;
        camera.eye = Vector4(0, 0, 3);
        camera.origin = Vector4(0, 0, 0);
        camera.up = Vector4(0, 1, 0);
    }
    
    void Move(float x, float z) {
        const float speed = .1f;
        position = position + Vector4(x, 0.0, z).scale(speed);
    }

    void NewRotate(float y, float z) {
        //std::cout << "Rotation: " << lookDir.to_string() << std::endl;
        const float speed = .1f;
        //lookDir = Vector4(0, sin(y), cos(z));
        
    }

    void Update() {
        camera.origin = position;
        camera.eye = position + lookDir;
    }
};

#endif