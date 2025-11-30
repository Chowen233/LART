// LART: Linear Algebra-based Ray Tracing

#include "LART.h"

#include "camara.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

int main() {
	hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.8));
    auto material_center = make_shared<lambertian>(color(0.2, 0.3, 0.5));
    //auto material_left = make_shared<metal>(color(0.8, 0.8, 0.8), 0.1);
    auto material_left = make_shared<dielectric>(1.33);
    auto material_right = make_shared<metal>(color(0.8, 0.6, 0.2), 0.3);

    world.add(make_shared<sphere>(point3(0.0, -1000.5, -1.5), 1000.0, material_ground));
    world.add(make_shared<sphere>(point3(0.0, 0.0, -1.8), 0.5, material_center));
    world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.5), 0.5, material_left));
    world.add(make_shared<sphere>(point3(1.0, 0.0, -1.5), 0.5, material_right));

    camera cam;

    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 1920;
    cam.samples_per_pixel = 200;
    cam.max_depth = 100;

    cam.render(world);
}
