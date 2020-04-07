#ifndef _CSCI441_PLAYER_
#define _CSCI441_PLAYER_

#include <glad/glad.h>
#include <csci441/matrix4.h>
#include <csci441/vector4.h>
#include <csci441/matrix3.h>
#include <csci441/uniform.h>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

#include "camera.h"
#include "shape.h"

class Player {
public:
    Camera camera;
    
    Vector4 position = Vector4(1,0,1);
    Vector4 lookDir = Vector4(0,0,-1);
    Vector4 rotate = Vector4();
    
    bool viewAbove = false;
    float prevMousePos[2] = { 0.0, 0.0 };

    Sphere model = Sphere(20, 1);
    Matrix4 transform;
    GLuint vao, vbo;

    Player() {
        
        // setup camera
        Matrix4 projection;
        projection.perspective(45, 1, .01, 10);
        camera = Camera();
        camera.projection = projection;
        camera.up = Vector4(0, 1, 0);

        Update();
    }

    // setup buffers
    void initPlayer() {
        glGenBuffers(1, (&vbo));
        glGenVertexArrays(1, (&vao));

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, model.size(), model.data(), GL_STATIC_DRAW);

        glBindVertexArray(vao);

        // setup position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }
    
    // move player's position
    void Move(float x, float z) {
        const float speed = .02f;
        position = position + (lookDir.scale(z) + lookDir.cross(Vector4(0, 1, 0)).scale(x)).scale(speed);
    }

    // rotate player's lookDir
    void Rotate(float x) {
        const float speed = .002f;
        x = (x * speed) - 180;
        
        lookDir = Vector4(cos(x), 0, sin(x)).normalized();
    }

    // apply player's position, lookDir, and viewAbove to camera and model transform
    void Update() {
        transform.set_to_identity();
        Matrix4 t;
        Matrix4 s;
        Matrix4 r;

        t.translate(position.x(), -0.3f, position.z());
        s.scale(.1f, .2f, .1f);
        transform = t * s * transform;
        
        if (viewAbove) {
            camera.origin = position + Vector4(0, 2, 0);
            camera.up = lookDir;
            camera.eye = position + Vector4(0, 3, 0);
        }
        else {
            camera.eye = position - lookDir;
            camera.origin = position;
            camera.up = Vector4(0, 1, 0);
        }
        
    }

    void Render(int shaderID, int tex) {
        Matrix3 it;
        it.inverse_transpose(transform);
        Uniform::set(shaderID, "model", transform);
        Uniform::set(shaderID, "itModel", it);
        Uniform::set(shaderID, "ourTexture", tex);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, model.size());
    }
};

#endif