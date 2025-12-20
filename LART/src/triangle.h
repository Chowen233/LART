#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "hittable_list.h"

class triangle : public hittable {
  public:
    triangle(const point3& v0, const point3& v1, const point3& v2, shared_ptr<material> mat)
        : v0(v0), v1(v1), v2(v2), mat(mat)
    {
        E1 = v1 - v0;
        E2 = v2 - v0;
        normal = unit_vector(cross(E1, E2));
        bbox = aabb(v0, v1, v2);
    }

    aabb bounding_box() const override { return bbox; }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        auto P = cross(r.direction(), E2);
        double det = dot(E1, P);

        if (std::fabs(det) < 1e-8)
            return false;

        double invDet = 1.0 / det;
        auto T = r.origin() - v0;
        
        auto u = dot(T, P) * invDet;
        if (u < 0.0 || u > 1.0)
            return false;

        auto Q = cross(T, E1);
        auto v = dot(r.direction(), Q) * invDet;
        if (v < 0.0 || u + v > 1.0)
            return false;

        auto t = dot(E2, Q) * invDet;

        if (!ray_t.contains(t))
            return false;

        // Ray hits the 2D shape; set the rest of the hit record and return true.
        rec.t = t;
        rec.p = r.at(t);
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }
  private:
    point3 v0, v1, v2;
    vec3 E1, E2;
    point3 normal;
    shared_ptr<material> mat;
    aabb bbox;
};

#endif