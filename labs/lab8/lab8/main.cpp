#include <iostream>

#include <glm/glm.hpp>

#include <bitmap/bitmap_image.hpp>

struct Viewport {
    glm::vec2 min;
    glm::vec2 max;

    Viewport(const glm::vec2& min, const glm::vec2& max)
        : min(min), max(max) {}
};

struct Sphere {
    int id;
    glm::vec3 center;
    glm::vec3 color;
    float radius;

    Sphere(const glm::vec3& center=glm::vec3(0,0,0),
            float radius=0,
            const glm::vec3& color=glm::vec3(0,0,0))
        : center(center), radius(radius), color(color) {
            static int id_seed = 0;
            id = ++id_seed;
        }
};

struct Ray {
    glm::vec3 origin;
    glm::vec3 dir;

    Ray(const glm::vec3& origin, const glm::vec3& dir) 
        : origin(origin), dir(dir) {}
};

glm::vec3 intersect(const std::vector<Sphere>& world, const Ray& ray) {

    Sphere closestIntersect;
    float renderDist = 100;
    float t0 = renderDist;
    for (int i = 0; i < world.size(); i++)
    {
        //find a,b,c for sphere s
        Sphere s = world[i];
        float a = glm::dot(ray.dir, ray.dir); //this *should* always be 1
        float b = 2 * glm::dot(ray.dir, ray.origin - s.center);
        float c = glm::dot(ray.origin - s.center, ray.origin - s.center) - (s.radius * s.radius);

        float discr = (b * b) - (4 * a *c); //find discriminant

        //if the ray intersects the sphere
        if (discr >= 0) {
            discr = sqrt(discr);
            float t1 = (-b + discr) / (2.0*a); //far point
            float t2 = (-b - discr) / (2.0*a); //near point
            float t = 0;

            //find closest intersection for t >= 0
            if (t2 >= 0)
            {
                t = t2;
            }
            else if (t1 >= 0){
                t = t2;
            }
            else {
                continue; //both intersections are behind the origin
            }
            //if t is closer than previous closest t, assign new closest values
            if (t < t0) {
                t0 = t;
                closestIntersect = s;
            }
        }
    }
    //return gray if no intersection was found
    if (t0 == renderDist) {
        return glm::vec3(0.2,0.2,0.2);
    }

    //return closest sphere's color if intersection was found 
    else {
        return (closestIntersect).color;
    }
}


void render(bitmap_image& image, const std::vector<Sphere>& world, float d) {
    Viewport view(glm::vec2(-5.0, -5.0), glm::vec2(5.0, 5.0));
    const int nx = 640;
    const int ny = 480;

    for (int x = 0; x < nx; x++) {
        for (int y = 0; y < ny; y++) {

            //find point at pixel (x,y)
            float ui = view.min.x + ((view.max.x - view.min.x) * ((float(x) + 0.5) / float(nx))); //left = min
            float vj = view.max.y + ((view.min.y - view.max.y) * ((float(y) + 0.5) / float(ny))); //bottom = max
            Ray ray (glm::vec3(ui, vj, 0.0), glm::vec3(0.0, 0.0, 1.0)); //ray for ortho

            //ray for perspective
            if (d > 0) {
                ray = Ray(glm::vec3(0.0, 0.0, -5.0), glm::normalize(glm::vec3(ui, vj, d))); //camera: (0,0,-5)
            }
            
            //find intersection
            glm::vec3 color = intersect(world, ray);

            //translate (1.0,1.0,1.0)rgb to (255,255,255)rgb
            rgb_t c;
            c.red = int(color.x * 255);
            c.green = int(color.y * 255);
            c.blue = int(color.z * 255);

            image.set_pixel(x, y, c); //set pixel color
        }
    }
}

int main(int argc, char** argv) {

    // create an image 640 pixels wide by 480 pixels tall
    bitmap_image image(640, 480);

    // build world
    std::vector<Sphere> world = {
        Sphere(glm::vec3(0, 0, 1), 1, glm::vec3(1,1,0)),
        Sphere(glm::vec3(1, 1, 4), 2, glm::vec3(0,1,1)),
        Sphere(glm::vec3(2, 2, 6), 3, glm::vec3(1,0,1)),
    };

    // render the world in ortho
    render(image, world, 0);
    image.save_image("ray-traced-ortho.bmp");

    // render the world in perspective
    render(image, world, 10.0);
    image.save_image("ray-traced-perspective.bmp");

    std::cout << "Success" << std::endl;
}


