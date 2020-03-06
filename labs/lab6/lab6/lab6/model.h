#ifndef _CSCI441_MODEL_H_
#define _CSCI441_MODEL_H_

#include <csci441/vector4.h>
#include <string>

class Point {
public:
    Vector4 pos, col, norm;
    Point(const Vector4 position=Vector4(), const Vector4 color=Vector4(), const Vector4 normal=Vector4()) : pos(position), col(color), norm(normal) {}

};

class Triangle {
public:
    Vector4 verticies[3];
    Triangle(const Vector4& a, const Vector4& b, const Vector4& c) {
        verticies[0] = a;
        verticies[1] = b;
        verticies[2] = c;
    }
};

class Model {

public:
    GLuint vbo;
    GLuint vao;
    GLuint ebo;
    Shader shader;
    Matrix4 model;
    int size;
    bool flat;

    template <typename Coords, typename Triangles>
    Model(const Coords coords, const Triangles indices, const Shader& shader_in, const bool flatShading) : shader(shader_in), flat(flatShading) {

        std::vector<Point> points;
        std::vector<float> newCoords;
        float pos[3], col[3];
        int n = 0;

        for (int i = 0; i < coords.size(); i++) {
            n = i % 6;
            if (n < 3) {
                pos[n] = coords[i];
            }
            else {
                col[n - 3] = coords[i];
                if (n == 5) {
                    Vector4 position(pos[0], pos[1], pos[2]);
                    Vector4 color(col[0], col[1], col[2]);
                    
                    points.push_back(Point(position, color));
                    std::cout << points.back().pos << points.back().col << std::endl;
                }
            }
        }
        if (flat) {

            Point triPoints[3];
            Vector4 normal;

            for (int i = 0; i < indices.size(); i++) {
                n = i % 3;
                triPoints[n] = points[indices[i]];
                if (n == 2) {
                    Triangle triangle(triPoints[0].pos, triPoints[1].pos, triPoints[2].pos);
                    normal = TriangleNormal(triangle.verticies);
                    for (int j = 0; j < 3; j++)
                    {
                        newCoords.push_back(triPoints[j].pos.x());
                        newCoords.push_back(triPoints[j].pos.y());
                        newCoords.push_back(triPoints[j].pos.z());
                        newCoords.push_back(triPoints[j].col.x());
                        newCoords.push_back(triPoints[j].col.y());
                        newCoords.push_back(triPoints[j].col.z());
                        newCoords.push_back(normal.x());
                        newCoords.push_back(normal.y());
                        newCoords.push_back(normal.z());
                    }
                }
            }
        }
        else {

            //find normal for each triangle
            std::vector<Triangle> tris;
            std::vector<Vector4> normals;
            Vector4 triVerts[3];
            std::cout << "finding triangle normals" << std::endl;
            for (int i = 0; i < indices.size(); i++) {
                n = i % 3;
                triVerts[n] = points[indices[i]].pos;
                if (n == 2) {
                    std::cout << "found normal for triangle " << i / 3 << std::endl;
                    tris.push_back(Triangle(triVerts[0], triVerts[1], triVerts[2]));
                    normals.push_back(TriangleNormal(triVerts));
                }
            }
            //find normal for each point
            for (int i = 0; i < points.size(); i++) {
                
                std::vector<Triangle> triangles;
                int count = 0;
                Vector4 normal = Vector4();
                for (int j = 0; j < tris.size(); j++) {
                    for (int k = 0; k < 3; k++) {
                        if (tris[j].verticies[k] == points[i].pos) {
                            std::cout << "in triangle " << j << " normal is: " << normals[j] << std::endl;
                            normal = normal + normals[j];
                            count++;
                            continue;
                        }
                    }
                }
                normal.values[3] = 0;
                normal = normal.scale(1.0 / double(count)).normalized();
                points[i].norm = normal;
                std::cout << "found normal for point " << i << ": " << normal << std::endl;
                
            }
            std::cout << "found normals" << std::endl;
            
            Point triPoints[3];
            for (int i = 0; i < indices.size(); i++) {
                n = i % 3;
                triPoints[n] = points[indices[i]];
                if (n == 2) {
                    for (int j = 0; j < 3; j++)
                    {
                        newCoords.push_back(triPoints[j].pos.x());
                        newCoords.push_back(triPoints[j].pos.y());
                        newCoords.push_back(triPoints[j].pos.z());
                        newCoords.push_back(triPoints[j].col.x());
                        newCoords.push_back(triPoints[j].col.y());
                        newCoords.push_back(triPoints[j].col.z());
                        newCoords.push_back(triPoints[j].norm.x());
                        newCoords.push_back(triPoints[j].norm.y());
                        newCoords.push_back(triPoints[j].norm.z());
                    }
                }
            }
        }

        size = newCoords.size() * sizeof(float);

        // copy vertex data
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, size, newCoords.data(), GL_STATIC_DRAW);

        // describe vertex layout
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
            (void*)(0 * sizeof(float)));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
            (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float),
            (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        
    }

    Vector4 TriangleNormal(const Vector4* verts) {;
        
        Vector4 ab = verts[1] - verts[0];
        Vector4 ac = verts[2] - verts[0];

        std::cout << verts[0] << verts[1] << verts[2] << ab << ac << std::endl;
        
        return ab.cross(ac);
    }
};

#endif
