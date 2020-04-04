#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>



#include <csci441/shader.h>
#include <csci441/matrix4.h>
#include <csci441/matrix3.h>
#include <csci441/vector4.h>
#include <csci441/uniform.h>

#include "shape.h"
#include "camera.h"
#include "player.cpp"
#include "map.cpp"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 960;

Player player;
Map map;

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

bool isPressed(GLFWwindow* window, int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

bool isReleased(GLFWwindow* window, int key) {
    return glfwGetKey(window, key) == GLFW_RELEASE;
}

Matrix4 translateModel(const Matrix4& model, const Vector4& translation) {
    translation.scale(.01);

    Matrix4 transform;
    transform.translate(translation.x(), translation.y(), translation.z());
    return transform * model;
}

Matrix4 rotateModel(const Matrix4& model, const Vector4& rotation) {
    Matrix4 transform;
    transform.translate(rotation.x(), rotation.y(), rotation.z());
    return transform * model;
}
Matrix4 processModel(const Matrix4& model, GLFWwindow* window) {
    Matrix4 trans;

    const float ROT = 1;
    const float SCALE = .05;
    const float TRANS = .01;

    // ROTATE
    if (isPressed(window, GLFW_KEY_U)) { trans.rotate_x(-ROT); }
    else if (isPressed(window, GLFW_KEY_I)) { trans.rotate_x(ROT); }
    else if (isPressed(window, GLFW_KEY_O)) { trans.rotate_y(-ROT); }
    else if (isPressed(window, GLFW_KEY_P)) { trans.rotate_y(ROT); }
    else if (isPressed(window, '[')) { trans.rotate_z(-ROT); }
    else if (isPressed(window, ']')) { trans.rotate_z(ROT); }
    // SCALE
    else if (isPressed(window, '-')) { trans.scale(1 - SCALE, 1 - SCALE, 1 - SCALE); }
    else if (isPressed(window, '=')) { trans.scale(1 + SCALE, 1 + SCALE, 1 + SCALE); }
    // TRANSLATE
    else if (isPressed(window, GLFW_KEY_UP)) { trans.translate(0, TRANS, 0); }
    else if (isPressed(window, GLFW_KEY_DOWN)) { trans.translate(0, -TRANS, 0); }
    else if (isPressed(window, GLFW_KEY_LEFT)) { trans.translate(-TRANS, 0, 0); }
    else if (isPressed(window, GLFW_KEY_RIGHT)) { trans.translate(TRANS, 0, 0); }
    else if (isPressed(window, ',')) { trans.translate(0, 0, TRANS); }
    else if (isPressed(window, '.')) { trans.translate(0, 0, -TRANS); }

    return trans * model;
}

bool isSpaceEvent(GLFWwindow* window) {
    static bool pressed = false;

    bool trigger = false;
    if (isPressed(window, GLFW_KEY_SPACE)) {
        pressed = true;
    }
    else if (pressed && isReleased(window, GLFW_KEY_SPACE)) {
        pressed = false;
        trigger = true;
    }
    return trigger;
}

void processInput(GLFWwindow* window) {
    if (isPressed(window, GLFW_KEY_ESCAPE) || isPressed(window, GLFW_KEY_Q)) {
        glfwSetWindowShouldClose(window, true);
    }
    else {
        //handle player movement
        if (isPressed(window, GLFW_KEY_A)) {
            player.Move(-1, 0);
        }
        if (isPressed(window, GLFW_KEY_D)) {
            player.Move(1, 0);
        }
        if (isPressed(window, GLFW_KEY_S)) {
            player.Move(0, -1);
        }
        if (isPressed(window, GLFW_KEY_W)) {
            player.Move(0, 1);
        }

        //handle player look
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        player.Rotate(xpos);

        player.Update();
    }
}

void errorCallback(int error, const char* description) {
    fprintf(stderr, "GLFW Error: %s\n", description);
}



int main(void) {
    GLFWwindow* window;

    glfwSetErrorCallback(errorCallback);
    
    /* Initialize the library */
    if (!glfwInit()) { return -1; }

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CSCI441-Project1", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // tell glfw what to do on resize
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // init glad
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize OpenGL context" << std::endl;
        glfwTerminate();
        return -1;
    }

    // init shader
    Shader shader("vert.glsl", "frag.glsl");

    // and use z-buffering
    glEnable(GL_DEPTH_TEST);


    // init the player and map
    player = Player();
    map = Map();
    map.initMap();

    // setup the textures
    glActiveTexture(GL_TEXTURE0);
    GLuint textureWall = loadTexture("textures/wallTexture1.png");
    glBindTexture(GL_TEXTURE_2D, textureWall);

    /*/ copy vertex data
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, wall.size(), wall.data(), GL_STATIC_DRAW);

    // describe vertex layout
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // setup position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        // process input
        processInput(window);

        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // render the object
        shader.use();
        
        
        Uniform::set(shader.ID, "projection", player.camera.projection);
        Uniform::set(shader.ID, "camera", player.camera.look_at());
        Uniform::set(shader.ID, "eye", player.camera.eye);
        Uniform::set(shader.ID, "ourTexture", 0);
        map.renderMap(shader.ID);
        /* Swap front and back and poll for io events */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
