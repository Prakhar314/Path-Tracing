#include "material.hpp"
#include <algorithm>
#include <glm/glm.hpp>
#include <math.h>
#include <utility>

using namespace std;

class Shape {
public:
  explicit Shape(const glm::vec3 albedo,
                 const glm::vec3 intensity = glm::vec3(0))
      : albedo(albedo / max(albedo.x, max(albedo.y, max(albedo.z, 1.0f)))),
        intensity(intensity), is_light(glm::length(intensity) > 0.001f) {}

  virtual float intersect(const glm::vec3 &o, const glm::vec3 &d,
                          const float t_min, const float t_max,
                          glm::vec3 &hit_normal) const = 0;

  virtual glm::vec3 get_position() const { throw "Not implemented"; }

  Material *material = new Transparent(1.0f, false);
  const glm::vec3 albedo;

  const glm::vec3 intensity;
  const bool is_light;
  virtual glm::vec3 get_le(glm::vec3 ray_origin,
                           glm::vec3 ray_intersection) const {
    return intensity;
  }

  ~Shape() { delete material; }
};

class Sphere : public Shape {
public:
  Sphere(const glm::vec3 &center, const float radius, const glm::vec3 albedo,
         const glm::vec3 intensity = glm::vec3(0))
      : Shape(albedo, intensity), center(center), radius(radius) {}
  glm::vec3 get_position() const override;
  float intersect(const glm::vec3 &o, const glm::vec3 &d, const float t_min,
                  const float t_max, glm::vec3 &hit_normal) const override;

private:
  const glm::vec3 center;
  const float radius;
};

class Plane : public Shape {
public:
  Plane(const glm::vec3 &normal, const float d, const glm::vec3 albedo,
        const glm::vec3 intensity = glm::vec3(0))
      : Shape(albedo, intensity), normal(normal), d(d) {}
  float intersect(const glm::vec3 &o, const glm::vec3 &d, const float t_min,
                  const float t_max, glm::vec3 &hit_normal) const override;

private:
  const glm::vec3 normal;
  const float d;
};

class Cuboid : public Shape {
public:
  Cuboid(const glm::vec3 &min, const glm::vec3 &max, const glm::vec3 albedo,
         const glm::vec3 intensity = glm::vec3(0))
      : Shape(albedo, intensity), min_coords(min), max_coords(max) {
    // ensure all coordinates are in the correct order
    assert(min_coords.x <= max_coords.x);
    assert(min_coords.y <= max_coords.y);
    assert(min_coords.z <= max_coords.z);
  }
  float intersect(const glm::vec3 &o, const glm::vec3 &d, const float t_min,
                  const float t_max, glm::vec3 &hit_normal) const override;

private:
  const glm::vec3 min_coords;
  const glm::vec3 max_coords;
};

class Point : public Shape {
public:
  Point(const glm::vec3 &position, const glm::vec3 albedo = glm::vec3(0),
        const glm::vec3 intensity = glm::vec3(0))
      : Shape(albedo, intensity), position(position) {}
  float intersect(const glm::vec3 &o, const glm::vec3 &d, const float t_min,
                  const float t_max, glm::vec3 &hit_normal) const override;
  glm::vec3 get_position() const override;
  glm::vec3 get_le(glm::vec3 ray_origin,
                   glm::vec3 ray_intersection) const override;

private:
  const glm::vec3 position;
};
