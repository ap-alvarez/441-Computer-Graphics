#ifndef _CSCI441_PLAYER_
#define _CSCI441_PLAYER_

#include <csci441/matrix4.h>
#include <csci441/vector4.h>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

#include "camera.h"
#include "shape.h"

class Player {
public:
    Camera camera;
    
    Vector4 position = Vector4(0.0,0,-1.0f);
    Vector4 lookDir = Vector4(0,0,1);
    Sphere model = Sphere(20, 1);
    Matrix4 transform;
    bool viewAbove = false;
    float prevMousePos[2] = { 0.0, 0.0 };

    Player() {
        
        // setup camera
        Matrix4 projection;
        projection.perspective(45, 1, .01, 10);
        camera = Camera();
        camera.projection = projection;
        camera.up = Vector4(0, 1, 0);
        Update();
    }
    
    void Move(float x, float z) {
        const float speed = .02f;
        position = position + (lookDir.scale(z) + lookDir.cross(Vector4(0, 1, 0)).scale(x)).scale(speed);
    }

    void Rotate(float x) {
        const float speed = .002f;
        x = x * speed;
        
        lookDir = Vector4(cos(x), 0, sin(x)).normalized();
    }

    void Update() {
        transform = Matrix4();
        transform.translate(position.x(), position.y(), position.z());
        transform.rotate_x(acos(lookDir * Vector4(0, 0, 1)));
        camera.eye = position;
        camera.origin = position + lookDir;
    }
};

#endif