#ifndef _CSCI441_RENDERER_H_
#define _CSCI441_RENDERER_H_

#include <iostream>

#include "camera.h"
#include "hit.h"
#include "light.h"
#include "ray.h"
#include "intersector.h"

#define MAX_REF_COUNT 20

class Renderer {

    Intersector* _intersector;

    int clamp255(float v) {
        return std::max(0.0f, std::min(255.0f, 255.0f*v));
    }


    rgb_t to_color(const glm::vec3 c) {
        return make_colour(clamp255(c.x), clamp255(c.y), clamp255(c.z));
    }


    glm::vec3 phong(const Hit& hit, const Light* light, const glm::vec3& eye) {

        float ambientStrength = 0.2;
        float specularStrength = 0.5;
        float shinyness = 2;

        glm::vec3 pos = hit.position();
        glm::vec3 normal = hit.normal();

        glm::vec3 light_dir = glm::normalize(light->direction(pos));

        float ambient = ambientStrength;

        float diffuse = glm::max(glm::dot(normal, light_dir), 0.0f);

        glm::vec3 view_dir = glm::normalize(eye - pos);
        glm::vec3 reflectDir = glm::reflect(-light_dir, normal);
        float specular = specularStrength *
            pow(std::fmax(glm::dot(view_dir, reflectDir), 0.0), shinyness);

        glm::vec3 light_color  =
            (ambient+diffuse+specular)
            * light->attenuation(pos)
            * light->color();

        return light_color*hit.color();
    }


    glm::vec3 shade(const Camera& camera, const Lights& lights, const Hit& hit) {
        glm::vec3 color = camera.background;
        if (hit.is_intersection()) {
            color = glm::vec3(0,0,0);
            for (auto light : lights) {
                color += phong(hit, light, camera.pos);
            }
        }
        return color;
    }

    glm::vec3 render_pixel(
        const Camera& camera,
        const Lights& lights,
        const World& world,
        const Ray& ray
    ) {
        return find_color(camera, lights, world, ray, 0);
    }

    glm::vec3 find_color(
        const Camera& camera,
        const Lights& lights,
        const World& world,
        const Ray& ray,
        int i
    ) {
        Hit hit = _intersector->find_first_intersection(world, ray);
        //base case: if the hit is at max reflections, or it is not an intersection, or the hit is neither reflective nor refractive, return the shaded color of the hit
        if (i >= MAX_REF_COUNT || !hit.is_intersection() || (hit.is_intersection() && hit.reflection() == 0 && hit.refraction() == 0)) {
            return shade(camera, lights, hit);
        }
        else {
            i++;
            if (hit.refraction() > 0)
            {
                std::cout << "REFRACT" << std::endl;
                float R = fresnel_reflection(ray, hit);
                //return the weighted combination or reflection and refraction
                return ((1 - R) * find_color(camera, lights, world, find_refraction(hit, ray, world), i)) + (R * find_color(camera, lights, world, find_reflection(hit, ray, world), i));
            }
            else {
                glm::vec3 color = shade(camera, lights, hit) * (1-hit.reflection());
                std::cout << "REFLECT " << hit.reflection() << std::endl;
                //return the weighted combination of the reflection and shaded color of the surface
                return color + (find_color(camera, lights, world, find_reflection(hit,ray,world), i) * hit.reflection());
            }
        }
    }

    Ray find_reflection(
        const Hit& reflector,
        const Ray& ray,
        const World& world
    ) {
        glm::vec3 refDir = ray.direction - (2 * glm::dot(ray.direction, reflector.normal()) * reflector.normal());
        Ray newRay;
        newRay.origin = reflector.position();
        newRay.direction = refDir;
        
        return newRay;

    }
    float fresnel_reflection(const Ray& ray, const Hit refractor) {
        float cos_i = glm::dot(ray.direction, refractor.normal());
        
        //get variables
        if (cos_i < -1) { cos_i = -1; }
        else if (cos_i > 1) { cos_i = 1; }
        float n_1 = 1, n_2 = refractor.refraction();
        glm::vec3 N = refractor.normal();
        if (cos_i < 0) { cos_i = -cos_i; }
        else { std::swap(n_1, n_2); N = -refractor.normal(); }
        float eta = n_1 / n_2;
        float sin2t = eta * eta * (1 - cos_i * cos_i);
        float cos_t = sqrt(1.0 - sin2t);

        //aply formulas
        float R1 = ((n_1 * cos_i) - (n_2 * cos_t)) / ((n_1 * cos_i) + (n_2 * cos_t));
        float R2 = ((n_2 * cos_i) - (n_1 * cos_t)) / ((n_2 * cos_i) + (n_1 * cos_t));
        return ((R1 * R1) + (R2 * R2)) / 2;
    }

    Ray find_refraction(
        const Hit& refractor,
        const Ray& ray,
        const World& world
    ) {
        //get variables
        float cos_i = glm::dot(ray.direction, refractor.normal());
        if (cos_i < -1) { cos_i = -1; }
        else if (cos_i > 1) { cos_i = 1; }
        float n_1 = 1, n_2 = refractor.refraction();
        glm::vec3 N = refractor.normal();
        if (cos_i < 0) { cos_i = -cos_i; }
        else { std::swap(n_1, n_2); N = -refractor.normal(); }
        float eta = n_1 / n_2;
        float sin2t = eta * eta * (1 - (cos_i * cos_i));
        //find ray
        Ray refractedRay;

        //below critical angle
        if (sin2t <= 1) {
            refractedRay.origin = refractor.position();
            refractedRay.direction = (eta * ray.direction) + (((eta * cos_i) - sqrtf(1 - sin2t)) * N);

        }
        //above critical angle
        else {
            refractedRay = find_reflection(refractor, ray, world);
        }
        
        return refractedRay;
    }

public:

    Renderer(Intersector* intersector) : _intersector(intersector) { }

    void render(
        bitmap_image &image,
        const Camera& camera,
        const Lights& lights,
        const World& world
    ) {
        for (int y = 0; y < image.height(); ++y) {
            for (int x = 0; x < image.width(); ++x) {
                Ray ray = camera.make_ray(image.width(), image.height(), x, y);
                glm::vec3 c = render_pixel(camera, lights, world, ray);
                image.set_pixel(x, image.height()-y-1, to_color(c));
            }
        }
    }
};

#endif
