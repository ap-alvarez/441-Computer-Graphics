#include <iostream>
#include <typeinfo>

#include <glm/glm.hpp>

#include <bitmap/bitmap_image.hpp>
#include <time.h>

#include "camera.h"
#include "hit.h"
#include "intersector.h"
#include "light.h"
#include "ray.h"
#include "renderer.h"
#include "shape.h"
#include "timer.h"

class Node {
public:
    AlignedBox box;
    std::vector<Node> parent;
    std::vector<Node> children;
    Node() {}
    Node(const AlignedBox& box) : box(box) {}
    void appendChild(Node node) {
        children.push_back(node);
    }
    void appendParent(Node node) {
        parent.push_back(node);
    }
};

class BruteForceIntersector : public Intersector {
public:

    Hit find_first_intersection(const World& world, const Ray& ray) {
        Hit hit(ray);
        for (auto surface : world.shapes()) {
            double cur_t = surface->intersect(ray);

            if (cur_t < hit.t()) {
                hit.update(surface, cur_t);
            }
        }
        return hit;
    }
};


class MySimpleIntersector : public Intersector {
private:
    std::vector<AlignedBox> structure;
    bool predicate(const AlignedBox& box, const Ray& r) {

        
        float tmin, tmax, tymin, tymax, tzmin, tzmax;
        float ax = 1.0 / r.direction.x;
        if (ax >= 0) {
            tmin = (box.xMin() - r.origin.x) * ax;
            tmax = (box.xMax() - r.origin.x) * ax;
        } else {
            tmin = (box.xMax() - r.origin.x) * ax;
            tmax = (box.xMin() - r.origin.x) * ax;
        }
        float ay = 1.0 / r.direction.y;
        if (ay >= 0) {
            tymin = (box.yMin() - r.origin.y) * ay;
            tymax = (box.yMax() - r.origin.y) * ay;
        }
        else {
            tymin = (box.yMax() - r.origin.y) * ay;
            tymax = (box.yMin() - r.origin.y) * ay;
        }

        if ((tmin > tymax) || (tymin > tmax))
             return false;

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;

        float az = 1.0 / r.direction.z;
        if (az >= 0) {
            tzmin = (box.zMin() - r.origin.z) * az;
            tzmax = (box.zMax() - r.origin.z) * az;
        }
        else {
            tzmin = (box.zMax() - r.origin.z) * az;
            tzmax = (box.zMin() - r.origin.z) * az;
        }
        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;

        if (tzmax < tmax)
            tmax = tzmax;

        return true;
    }
public:
    Hit find_first_intersection(const World& world, const Ray& ray) {
        Hit hit(ray);
        for (AlignedBox box : structure) {
            if (predicate(box, ray)) {
                //std::cout << "oi" << std::endl;
                for (auto surface : world.shapes()) {
                    if (std::find(box.ids.begin(), box.ids.end(), surface->id()) != box.ids.end()) {
                        double cur_t = surface->intersect(ray);
                        
                        if (cur_t < hit.t()) {
                            hit.update(surface, cur_t);
                        }
                    }
                }
            }
        }
        return hit;
    }

    void appendBound(AlignedBox& box) {
        structure.push_back(box);
    }
    
};

class MyBestIntersector : public Intersector {
private:
    Node root = Node(AlignedBox(std::vector<int>(), glm::vec3(), 100.0f));
    bool predicate(const AlignedBox& box, const Ray& r) {


        float tmin, tmax, tymin, tymax, tzmin, tzmax;
        float ax = 1.0 / r.direction.x;
        if (ax >= 0) {
            tmin = (box.xMin() - r.origin.x) * ax;
            tmax = (box.xMax() - r.origin.x) * ax;
        }
        else {
            tmin = (box.xMax() - r.origin.x) * ax;
            tmax = (box.xMin() - r.origin.x) * ax;
        }
        float ay = 1.0 / r.direction.y;
        if (ay >= 0) {
            tymin = (box.yMin() - r.origin.y) * ay;
            tymax = (box.yMax() - r.origin.y) * ay;
        }
        else {
            tymin = (box.yMax() - r.origin.y) * ay;
            tymax = (box.yMin() - r.origin.y) * ay;
        }

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;

        float az = 1.0 / r.direction.z;
        if (az >= 0) {
            tzmin = (box.zMin() - r.origin.z) * az;
            tzmax = (box.zMax() - r.origin.z) * az;
        }
        else {
            tzmin = (box.zMax() - r.origin.z) * az;
            tzmax = (box.zMin() - r.origin.z) * az;
        }
        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;

        if (tzmax < tmax)
            tmax = tzmax;

        return true;
    }
    std::vector<Node> find_children_hit(const Node& parent, const Ray& r) {
        std::vector<Node> hit;
        for (Node child : parent.children) {
            if (predicate(child.box, r)) {
                hit.push_back(child);
            }
        }
        return hit;
    }
    std::vector<AlignedBox> find_leaves_hit(const Node& parent, const Ray& r) {
        std::vector<AlignedBox> hit;
        std::vector<Node> childrenHit = find_children_hit(parent, r);
        if (childrenHit.size() == 0) {
            hit.push_back(parent.box);
            return hit;
        }
        else {
            for (Node child : childrenHit) {
                std::vector<AlignedBox> leavesHit = find_leaves_hit(child, r);
                hit.insert(hit.end(), leavesHit.begin(), leavesHit.end());
            }
        }
        return hit;
    }
public:
    Hit find_first_intersection(const World& world, const Ray& ray) {
        Hit hit(ray);
        std::vector<AlignedBox> leavesHit = find_leaves_hit(root, ray);
        for (AlignedBox box : leavesHit) {
            for (auto surface : world.shapes()) {
                if (std::find(box.ids.begin(), box.ids.end(), surface->id()) != box.ids.end()) {
                    double cur_t = surface->intersect(ray);

                    if (cur_t < hit.t()) {
                        hit.update(surface, cur_t);
                    }
                }
            }
        }
        return hit;
    }
    

    void appendBound(AlignedBox& box) {
        root.appendChild(Node(box));
    }

};


double rand_val() {
    static bool init = true;
    if (init) {
        srand(time(NULL));
        init = false;
    }
    return ((double) rand() / (RAND_MAX));
}

glm::vec3 rand_color() {
    return glm::vec3(rand_val(),rand_val(),rand_val());
}



glm::vec4 random_box() {
    float  x = (rand_val() * 8) - 4;
    float  y = (rand_val() * 8) - 4;
    float  z = rand_val() * 5;
    float scale = rand_val() * 2;
    return glm::vec4(x,y,z,scale);
}


int main(int argc, char** argv) {

    // set the number of boxes
    int NUM_BOXES = 1;

    // create an image 640 pixels wide by 480 pixels tall
    bitmap_image image(640, 480);

    // setup the camera
    float dist_to_origin = 5;
    Camera camera(
            glm::vec3(0, 0, -dist_to_origin),   // eye
            glm::vec3(0, 0, 0),                 // target
            glm::vec3(0, 1, 0),                 // up
            glm::vec2(-5, -5),                  // viewport min
            glm::vec2(5, 5),                    // viewport max
            dist_to_origin,                     // distance from eye to view plane
            glm::vec3(.3, .6, .8)               // background color
    );

    // setup lights
    // see http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
    // for good attenuation value.
    // I found the values at 7 to be nice looking
    PointLight l1(glm::vec3(1, 1, 1), glm::vec3(3, -3, 0), 1.0, .7, 0.18);
    DirectionalLight l2(glm::vec3(.5, .5, .5), glm::vec3(-5, 4, -1));
    Lights lights = { &l1, &l2 };

    // create the intersector
    BruteForceIntersector intersector;
    MySimpleIntersector betterIntersector;
    MyBestIntersector bestIntersector;

    // and setup the renderer
    Renderer renderer(&betterIntersector);
    Renderer betterRenderer(&betterIntersector);
    Renderer bestRenderer(&bestIntersector);

    float milliseconds = 0;
    // render
    while (NUM_BOXES <= 4) {
        // setup world
        World world;

        // add the light
        Sphere light (l1.position(), .25, glm::vec3(1, 1, 1));
        std::vector<int> lightId;
        lightId.push_back(light.id());
        world.append(light);
        betterIntersector.appendBound(*&AlignedBox(lightId, l1.position(), 0.5f));
        //bestIntersector.appendBound(*&AlignedBox(lightId, l1.position(), 0.5f));
        // and the spheres
        std::vector<int> sphereIds;
        std::vector<Sphere> spheres;
        spheres.push_back(Sphere(glm::vec3(1, 1, 1), 1, rand_color()));
        spheres.push_back(Sphere(glm::vec3(2, 2, 4), 2, rand_color()));
        spheres.push_back(Sphere(glm::vec3(3, 3, 6), 3, rand_color()));
        sphereIds.push_back(spheres[0].id());
        sphereIds.push_back(spheres[1].id());
        sphereIds.push_back(spheres[2].id());
        world.append(spheres[0]);
        world.append(spheres[1]);
        world.append(spheres[2]);
        betterIntersector.appendBound(*&AlignedBox(sphereIds, glm::vec3(4.5f, 4.5f, 4.5f), 9.0f));
        //bestIntersector.appendBound(*&AlignedBox(sphereIds, glm::vec3(4.5f, 4.5f, 4.5f), 9.0f));

        // and add some boxes and prep world for rendering
        for (int i = 0; i < NUM_BOXES; ++i) {
            glm::vec4 box = random_box();
            std::vector<Triangle> tris = Obj::make_box(glm::vec3(box.x, box.y, box.z), box.w, rand_color());

            std::vector<int> ids;
            for (Triangle tri : tris) {
                ids.push_back(tri.id());
            }
            AlignedBox bound(ids, glm::vec3(box.x, box.y, box.z), box.w);
            
            betterIntersector.appendBound(bound);
            //bestIntersector.appendBound(bound);
            world.append(tris);
        }
        world.lock();

        Timer timer;
        timer.start();
        betterRenderer.render(image, camera, lights, world);
        timer.stop();
        milliseconds = timer.total();
        image.save_image("ray-traced-"+std::to_string(NUM_BOXES)+".bmp");
        std::cout << NUM_BOXES << " rendered in " << milliseconds << " milliseconds : " << (milliseconds/1000.0) << " seconds" << std::endl;
        NUM_BOXES++;
    }
}


