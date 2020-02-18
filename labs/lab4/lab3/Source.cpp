#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Matrix4.h"

#define PI 3.14159265

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

float H=0,Tx=0.0, Ty=0.0, Tz=0.0, S=1.0, Rx=0.0, Ry=0.0, Rz=.0;
bool ortho=true, changeO=false, changeB=false, isBox=true;


void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processinput(GLFWwindow* window, Shader& shader) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // SWITCH BUFFER

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        changeB = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE && changeB) {
        changeB = false;
        std::cout << "change shape" << std::endl;
        isBox = !isBox;
    }

    // CAMERA INPUT

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && H > -20.0f) {
        H -= 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && H < 20.0f) {
        H += 0.1;
    }

    // PROJECTION TYPE INPUT

    if (glfwGetKey(window, GLFW_KEY_BACKSLASH) == GLFW_PRESS) {
        changeO = true;
    }
    if (glfwGetKey(window, GLFW_KEY_BACKSLASH) == GLFW_RELEASE && changeO) {
        changeO = false;
        ortho = !ortho;
    }

    // TRANSLATION INPUT

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        Tx -= 0.05;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        Tx += 0.05;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        Ty -= 0.05;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        Ty += 0.05;
    }

    if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) {
        Tz -= 0.05;
    }
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
        Tz += 0.05;
    }

    // SCALING INPUT

    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
        S -= 0.05;
    }
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
        S += 0.05;
    }

    // ROTATION INPUT

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        Rx -= 0.05;
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        Rx += 0.05;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        Ry-= 0.05;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        Ry += 0.05;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS) {
        Rz -= 0.05;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS) {
        Rz += 0.05;
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

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lab 4", NULL, NULL);
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

    /* init the model */
    float box[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f
    };

    float cylinder[8 * 12 * 7];
    float theta = 0;
    float phi = 0;
    int i = 0;
    while (theta < 2 * PI) {

        phi = theta + (PI / 4.0);

        //first triangle (upper left)
        cylinder[i++] = 0.5 * cos(theta);
        cylinder[i++] = 0.5f;
        cylinder[i++] = 0.5 * sin(theta);
        cylinder[i++] = 1.0f;
        cylinder[i++] = 0.0f;
        cylinder[i++] = 0.0f;

        cylinder[i++] = 0.5 * cos(theta);
        cylinder[i++] = -0.5f;
        cylinder[i++] = 0.5 * sin(theta);
        cylinder[i++] = 1.0f;
        cylinder[i++] = 0.0f;
        cylinder[i++] = 0.0f;

        cylinder[i++] = 0.5 * cos(phi);
        cylinder[i++] = 0.5f;
        cylinder[i++] = 0.5 * sin(phi);
        cylinder[i++] = 1.0f;
        cylinder[i++] = 0.0f;
        cylinder[i++] = 0.0f;

        //second triangle (lower right)
        cylinder[i++] = 0.5 * cos(theta);
        cylinder[i++] = -0.5f;
        cylinder[i++] = 0.5 * sin(theta);
        cylinder[i++] = 0.0f;
        cylinder[i++] = 1.0f;
        cylinder[i++] = 0.0f;

        cylinder[i++] = 0.5 * cos(phi);
        cylinder[i++] = -0.5f;
        cylinder[i++] = 0.5 * sin(phi);
        cylinder[i++] = 0.0f;
        cylinder[i++] = 1.0f;
        cylinder[i++] = 0.0f;

        cylinder[i++] = 0.5 * cos(phi);
        cylinder[i++] = 0.5f;
        cylinder[i++] = 0.5 * sin(phi);
        cylinder[i++] = 0.0f;
        cylinder[i++] = 1.0f;
        cylinder[i++] = 0.0f;

        //top triangle
        cylinder[i++] = 0.5 * cos(theta);
        cylinder[i++] = 0.5f;
        cylinder[i++] = 0.5 * sin(theta);
        cylinder[i++] = 0.0f;
        cylinder[i++] = 0.0f;
        cylinder[i++] = 1.0f;

        cylinder[i++] = 0.5 * cos(phi);
        cylinder[i++] = 0.5f;
        cylinder[i++] = 0.5 * sin(phi);
        cylinder[i++] = 0.0f;
        cylinder[i++] = 0.0f;
        cylinder[i++] = 1.0f;

        cylinder[i++] = 0;
        cylinder[i++] = 0.5f;
        cylinder[i++] = 0;
        cylinder[i++] = 0.0f;
        cylinder[i++] = 0.0f;
        cylinder[i++] = 1.0f;

        //bottom triangle
        cylinder[i++] = 0.5 * cos(theta);
        cylinder[i++] = -0.5f;
        cylinder[i++] = 0.5 * sin(theta);
        cylinder[i++] = 0.0f;
        cylinder[i++] = 0.0f;
        cylinder[i++] = 1.0f;

        cylinder[i++] = 0.5 * cos(phi);
        cylinder[i++] = -0.5f;
        cylinder[i++] = 0.5 * sin(phi);
        cylinder[i++] = 0.0f;
        cylinder[i++] = 0.0f;
        cylinder[i++] = 1.0f;

        cylinder[i++] = 0;
        cylinder[i++] = -0.5f;
        cylinder[i++] = 0;
        cylinder[i++] = 0.0f;
        cylinder[i++] = 0.0f;
        cylinder[i++] = 1.0f;

        theta = phi;
    }

    // copy vertex data
    GLuint  VBOs[2];
    glGenBuffers(2, VBOs);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);


    // describe vertex layout
    GLuint VAOs[2];
    glGenVertexArrays(2, VAOs);
    glBindVertexArray(VAOs[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cylinder), cylinder, GL_DYNAMIC_DRAW);
    glBindVertexArray(VAOs[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAOs[0]);

    // describe vertex layout
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // create the shaders
    Shader shader("vert.glsl", "frag.glsl");

    // setup the textures
    shader.use();

    // and use z-buffering
    glEnable(GL_DEPTH_TEST);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        // process input
        processinput(window, shader);

        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        shader.use();
        
        float n = -10.0;
        float f = 10.0;
        float r = 1.0;
        float l = -1.0;
        float t = 1.0;
        float b = -1.0;

        float camera[16]{
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, -H,
            0.0, 0.0, 1.0, 20.0,
            0.0, 0.0, 0.0, 1.0 };

        float g = sqrt((H * H) + 400);

        float cameraRot[16]{
            1.0, 0.0, 0.0, 0.0,
            0.0, -20.0/g, -H/g, 0.0,
            0.0, -H/g, 20.0/g, 0.0,
            0.0, 0.0, 0.0, 0.0 };

        
        float orth[16]{
            2.0/(r-l), 0.0, 0.0, -((r+l)/(r-l)),
            0.0, 2.0/(t-b), 0.0, -((t+b)/(t-b)),
            0.0, 0.0, 2.0/(n-f), -((n+f)/(n-f)),
            0.0, 0.0, 0.0,       1.0 };
                             
        float pers[16]{
            n, 0.0, 0.0, 0.0,
            0.0, n, 0.0, 0.0,
            0.0, 0.0, n+f, -(n * f),
            0.0, 0.0, 2.0, 0.0 };

        Matrix4 proj;
        if (ortho) {
            proj = Matrix4(true);
        }
        else {
            proj = Matrix4(orth).MatMul(Matrix4(pers).MatMul(Matrix4(cameraRot).MatMul(Matrix4(camera))));
        }

        Matrix4 model = Translate(Tx, Ty, Tz).MatMul(Rotate(Rx, Ry, Rz).MatMul(Scale(S, S, S)));
        
        int modelLoc = glGetUniformLocation(shader.ID, "model");
        int projLoc = glGetUniformLocation(shader.ID, "perspective");
        glUniform1fv(modelLoc, 16, model.matrix);
        glUniform1fv(projLoc, 16, proj.matrix);

        if (isBox) {
            glBindVertexArray(VAOs[0]);
            glDrawArrays(GL_TRIANGLES, 0, sizeof(box));
        }
        else {
            glBindVertexArray(VAOs[1]);
            glDrawArrays(GL_TRIANGLES, 0, sizeof(cylinder));
        }
        
        

        /* Swap front and back and poll for io events */
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
