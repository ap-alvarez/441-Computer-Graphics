#ifndef _CSCI441_MAP_
#define _CSCI441_MAP_

#include <glad/glad.h>
#include <csci441/matrix4.h>
#include <csci441/vector4.h>

#include "shape.h"
#include "texture.h"

#define WALL_COUNT 61
#define MAP_SIZE 10


class Map {
public:
    GLuint vao[WALL_COUNT], vbo[WALL_COUNT], vaoF[MAP_SIZE * MAP_SIZE], vboF[MAP_SIZE * MAP_SIZE];
	Shape models[WALL_COUNT];
    Shape floors[MAP_SIZE*MAP_SIZE];

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

    Map() {}

    void initMap() {
		for (int i = 0; i < WALL_COUNT; i++) {
			models[i] = Cube();
		}
        for (int y = 0; y < MAP_SIZE; y++) {
            for (int x = 0; x < MAP_SIZE; x++) {
                floors[(y * MAP_SIZE) + x] = Cube();
            }
        }
        setupBuffers();
	}
    
    void renderMap(unsigned int shaderID, int wallTex, int floorTex) {
        int n = 0;
        for (int y = 0; y < 10; y++) {
            for (int x = 0; x < 10; x++) {

                //render the wall if there is one in the map
                if (map[y][x] != 0) {
                    Matrix4 t = Matrix4();
                    t.translate(float(-x), 0, float(-y));;
                    Matrix3 it;
                    it.inverse_transpose(t);
                    Uniform::set(shaderID, "model", t);
                    Uniform::set(shaderID, "itModel", it);
                    Uniform::set(shaderID, "ourTexture", wallTex);
                    glBindVertexArray(vao[n]);
                    glDrawArrays(GL_TRIANGLES, 0, models[n].size());
                    n++;
                }

                //render the floor
                Matrix4 floorT;
                floorT.translate(float(-x), -1, float(-y));
                Matrix3 floorIT;
                floorIT.inverse_transpose(floorT);
                Uniform::set(shaderID, "model", floorT);
                Uniform::set(shaderID, "itModel", floorIT);
                Uniform::set(shaderID, "ourTexture", floorTex);
                glBindVertexArray(vaoF[(y * MAP_SIZE) + x]);
                glDrawArrays(GL_TRIANGLES, 0, floors[(y * MAP_SIZE) + x].size());
            }
        }
    }

    void setupBuffers() {

        //wall buffers
        glGenBuffers(WALL_COUNT, vbo);
        glGenVertexArrays(WALL_COUNT, vao);
        for (int i = 0; i < WALL_COUNT; i++) {
            glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
            glBufferData(GL_ARRAY_BUFFER, models[i].size(), models[i].data(), GL_STATIC_DRAW);

            glBindVertexArray(vao[i]);

            // setup position attributes
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
            glEnableVertexAttribArray(2);
        }

        //floor buffers
        glGenBuffers(MAP_SIZE*MAP_SIZE, vboF);
        glGenVertexArrays(MAP_SIZE * MAP_SIZE, vaoF);
        for (int y = 0; y < MAP_SIZE; y++) {
            for (int x = 0; x < MAP_SIZE; x++) {
                glBindBuffer(GL_ARRAY_BUFFER, vboF[(y * MAP_SIZE) + x]);
                glBufferData(GL_ARRAY_BUFFER, floors[(y * MAP_SIZE) + x].size(), floors[(y * MAP_SIZE) + x].data(), GL_STATIC_DRAW);
            
                glBindVertexArray(vaoF[(y * MAP_SIZE) + x]);

                // setup position attributes
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);

                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
                glEnableVertexAttribArray(1);

                glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
                glEnableVertexAttribArray(2);
            }
        }
        
        

        
    }
};

#endif