#ifndef CAMERA_H
#define CAMERA_H

#include "hittable.h"
#include "material.h"

#include "progress_bar.h"
#include "denoiser.h"

#include <vector>
#include <omp.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "external/stb_image_write.h"

class camera {
  public:
    double aspect_ratio = 1.0;  // Ratio of image width over height
    int    image_width = 100;  // Rendered image width in pixel count
    int    samples_per_pixel = 10;   // Count of random samples for each pixel
    int    max_depth = 10;   // Maximum number of ray bounces into scene
    color  background;               // Scene background color

    int    max_samples_per_pixel = 100;
    int    min_samples_per_pixel = 10;

    double vfov = 90;  // Vertical view angle (field of view)
    point3 lookfrom = point3(0, 0, 0);   // Point camera is looking from
    point3 lookat = point3(0, 0, -1);  // Point camera is looking at
    vec3   vup = vec3(0, 1, 0);     // Camera-relative "up" direction

    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

    void render(const hittable& world) {
        initialize();

        int pixel_count = image_width * image_height;
        std::vector<unsigned char> image_color(pixel_count * 3);
        /*std::vector<unsigned char> image_albedo(pixel_count * 3);
        std::vector<unsigned char> image_normal(pixel_count * 3);*/

        std::vector<float> color_buffer(pixel_count * 3);
        std::vector<float> albedo_buffer(pixel_count * 3);
        std::vector<float> normal_buffer(pixel_count * 3);

        progress_bar bar(image_height);

        #pragma omp parallel for schedule(dynamic)
        for (int j = 0; j < image_height; j++) {
            for (int i = 0; i < image_width; i++) {
                color pixel_color (0, 0, 0);
                color pixel_albedo(0, 0, 0);
                color pixel_normal(0, 0, 0);

                for (int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    auto [albedo, normal] = ray_first_hit(r, world);
                    pixel_albedo += albedo;
                    pixel_normal += normal;
                }

                int sample_count = 0;

                for (int sample = 0; sample < max_samples_per_pixel; sample++) {
                    ray r = get_ray(i, j);
                    color color_tmp = ray_color(r, max_depth, world);
                    if (color_tmp[0] != 0 ||
                        color_tmp[1] != 0 ||
                        color_tmp[2] != 0) {
                        pixel_color += color_tmp;
                        if (++sample_count >= min_samples_per_pixel) {
                            total_sample_count += sample + 1;
                            break;
                        }
                    }
                }

                if (sample_count < min_samples_per_pixel) {
                    if (sample_count == 0)
                        sample_count = 1;
                    total_sample_count += max_samples_per_pixel;
                }

                int idx = (j * image_width + i) * 3;

                auto fill_buffer = [&](std::vector<float>& buffer, color pixel) {
                    buffer[idx]     = pixel[0];
                    buffer[idx + 1] = pixel[1];
                    buffer[idx + 2] = pixel[2];
                };

                fill_buffer(color_buffer, 1.0 / sample_count * pixel_color);
                fill_buffer(albedo_buffer, pixel_samples_scale * pixel_albedo);
                fill_buffer(normal_buffer, pixel_samples_scale * pixel_normal);

                /*write_color(1.0 / sample_count * pixel_color, image_color, idx);
                write_color(pixel_samples_scale * pixel_albedo, image_albedo, idx);
                write_color(pixel_samples_scale * (0.5 * pixel_normal + color(0.5, 0.5, 0.5)), image_normal, idx);*/
            }

            #pragma omp critical
            {
                bar.update(++progress);
            }
        }

        bar.end(1.0 * total_sample_count / pixel_count);

        /*write_png("image_color.png", image_color);
        write_png("image_albedo.png", image_albedo);
        write_png("image_normal.png", image_normal);*/

        //OIDN denoise
        denoise(color_buffer, albedo_buffer, normal_buffer, image_width, image_height);

        for (int i = 0; i < image_width * image_height; ++i) {
            int idx = i * 3;
            color pixel_color(color_buffer[idx], color_buffer[idx + 1], color_buffer[idx + 2]);
            write_color(pixel_color, image_color, idx);
        }

        write_png("image.png", image_color);
    }

  private:
    int    image_height;         // Rendered image height
    double pixel_samples_scale;  // Color scale factor for a sum of pixel samples
    point3 center;               // Camera center
    point3 pixel00_loc;          // Location of pixel 0, 0
    vec3   pixel_delta_u;        // Offset to pixel to the right
    vec3   pixel_delta_v;        // Offset to pixel below
    vec3   u, v, w;              // Camera frame basis vectors
    vec3   defocus_disk_u;       // Defocus disk horizontal radius
    vec3   defocus_disk_v;       // Defocus disk vertical radius

    int    progress = 0;
    long long total_sample_count = 0;

    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // Determine viewport dimensions.
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta / 2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the defocus disk and directed at a randomly
    // sampled point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                            + ((i + offset.x()) * pixel_delta_u)
                            + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    point3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return color(0, 0, 0);

        hit_record rec;

        // If the ray hits nothing, return the background color.
        if (!world.hit(r, interval(0.001, infinity), rec))
            return background;

        ray scattered;
        color attenuation;
        color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return color_from_emission;

        color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world);

        return color_from_emission + color_from_scatter;
    }

    // Get albedo and normal
    std::pair<color, vec3> ray_first_hit(const ray& r, const hittable& world) const {

        hit_record rec;

        if (world.hit(r, interval(0.001, infinity), rec)) {
            return { rec.mat->get_albedo(), rec.normal };
        }
        return { background, vec3(0, 0, 1) };
    }

    void write_png(std::string filename, std::vector<unsigned char>& image) {
        if (stbi_write_png(filename.c_str(), image_width, image_height, 3, image.data(), image_width * 3)) {
            std::cout << "\nWrote " << filename << "\n";
        }
        else {
            std::cout << "\nFailed to write " << filename << "\n";
        }
    }
};

#endif