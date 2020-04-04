#ifndef _CSCI441_MAP_
#define _CSCI441_MAP_

#include <glad/glad.h>
#include <csci441/matrix4.h>
#include <csci441/vector4.h>

#include "shape.h"
#include "texture.h"

#define WALL_COUNT 58


class Map {
public:
	//GLuint vaoWalls[WALL_COUNT], vboWalls[WALL_COUNT], uboWalls[WALL_COUNT];
    GLuint vao[WALL_COUNT], vbo[WALL_COUNT];
	Shape models[WALL_COUNT];
    Matrix4 transforms[WALL_COUNT];

    int map[10][10]{
        {1,0,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,1,0,0,0,1},
        {1,1,1,1,0,1,0,1,0,1},
        {1,1,1,1,0,1,0,1,1,1},
        {1,0,0,1,0,1,0,0,0,1},
        {1,0,1,1,0,1,1,1,0,1},
        {1,0,1,0,0,1,0,0,0,1},
        {1,0,1,0,1,1,0,1,0,1},
        {1,0,0,0,0,0,0,1,0,1},
        {1,1,1,1,1,1,1,1,0,1},
    };

    Map() {
    }

    void initMap() {
		for (int i = 0; i < WALL_COUNT; i++) {
			models[i] = Cube();
		}
        setupBuffers();
	}
    
    void renderMap(unsigned int shaderID) {
        int n = 0;
        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {
                if (map[y][x] != 0) {
                    Uniform::set(shaderID, "model", findTransform(x,y));
                    glBindVertexArray(vao[n]);
                    glDrawArrays(GL_TRIANGLES, 0, models[n].size());
                    n++;
                }
            }
        }
    }

    Matrix4 findTransform (int x, int y) {
        const float unit = .2f;
        Matrix4 tran;
        tran.translate(float(x), 0, float(y));
        //tran.scale(unit, 1, unit);
        
        return tran;
    }

    void setupTextures() {
        GLuint textureWall = loadTexture("textures/wallTexture1.png");
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureWall);
    }

    void setupBuffers() {
        /*glGenBuffers(WALL_COUNT, vboWalls);
        glGenVertexArrays(WALL_COUNT, vaoWalls);
        for (int i = 0; i < WALL_COUNT; i++) {


            glBindBuffer(GL_ARRAY_BUFFER, vboWalls[i]);
            glBufferData(GL_ARRAY_BUFFER, models[i].size(), models[i].data(), GL_STATIC_DRAW);


            glBindVertexArray(vaoWalls[i]);

            // setup position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }*/

        glGenBuffers(WALL_COUNT, vbo);
        glGenVertexArrays(WALL_COUNT, vao);
        for (int i = 0; i < WALL_COUNT; i++) {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
            glBufferData(GL_ARRAY_BUFFER, models[0].size(), models[i].data(), GL_STATIC_DRAW);

            glBindVertexArray(vao[i]);

            // setup position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
        }
        
    }
};

#endif