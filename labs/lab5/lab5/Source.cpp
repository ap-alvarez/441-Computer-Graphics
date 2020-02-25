#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <csci441/shader.h>
#include <csci441/matrix.h>
#include <csci441/vector.h>
#include <csci441/uniform.h>

#include "shape.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 960;

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

bool isPressed(GLFWwindow* window, int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

bool isReleased(GLFWwindow* window, int key) {
    return glfwGetKey(window, key) == GLFW_RELEASE;
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

void processInput(Matrix4& model, GLFWwindow* window) {
    if (isPressed(window, GLFW_KEY_ESCAPE) || isPressed(window, GLFW_KEY_Q)) {
        glfwSetWindowShouldClose(window, true);
    }
    model = processModel(model, window);
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
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "lab5", NULL, NULL);
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


    // create c
    Cylinder c(20, 1, .2, .4);
    //Cone c(20, 1, .2, .4);
    // Sphere c(20, .5, 1, .2, .4);
    // Torus c(20, .75, .25, 1, .2, .4);
    //DiscoCube c;

    // copy vertex data
    GLuint VBO1;
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, c.coords.size() * sizeof(float),
        &c.coords[0], GL_STATIC_DRAW);

    // describe vertex layout
    GLuint VAO1;
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 9 * sizeof(float),
        (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // setup projection
    Matrix4 projection;
    projection.perspective(45, 1, .01, 10);

    // setup view
    Vector4 eye(0.0, 0.0, -2.0);
    Vector4 origin(0, 0, 0);
    Vector4 up(0, 1, 0);

    Matrix4 camera;
    camera.look_at(eye, origin, up);

    // create the shaders
    Shader shader("vert.glsl", "frag.glsl");

    // setup the textures
    shader.use();

    // setup the model
    Matrix4 model;
    Matrix3 normalModel;

    // setup the light
    Vector4 lightPos(0.0f, 0.0, 2.0f);
    Vector4 lightColor(1.0f, 1.0f, 1.0f);
    Vector4 ambientColor(0.1f, 0.1f, 0.1f);

    // and use z-buffering
    glEnable(GL_DEPTH_TEST);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        // process input
        processInput(model, window);

        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        shader.use();

        //vertex shader uniforms
        normalModel.inverse_transpose(model);
        Uniform::set(shader.ID, "model", model);
        Uniform::set(shader.ID, "normalModel", normalModel);
        Uniform::set(shader.ID, "projection", projection);
        Uniform::set(shader.ID, "camera", camera);

        //fragment shader uniforms

        GLint loc = glGetUniformLocation(shader.ID, "lightPos");
        glUniform4fv(loc, 1, lightPos.values);
        loc = glGetUniformLocation(shader.ID, "lightColor");
        glUniform4fv(loc, 1, lightColor.values);
        loc = glGetUniformLocation(shader.ID, "ambient");
        glUniform4fv(loc, 1, ambientColor.values); 
        loc = glGetUniformLocation(shader.ID, "gazeDir");
        glUniform4fv(loc, 1, (origin-eye).values);
        loc = glGetUniformLocation(shader.ID, "specStrength");
        glUniform1f(loc, 0.2f);

        // render the cube
        glBindVertexArray(VAO1);
        glDrawArrays(GL_TRIANGLES, 0, c.coords.size() * sizeof(float));

        /* Swap front and back and poll for io events */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
