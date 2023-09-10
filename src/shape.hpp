#include "material.hpp"
#include <algorithm>
#include <glm/glm.hpp>
#include <math.h>
#include <utility>

using namespace std;

class Shape {
private:
  bool is_light;

public:
  Shape() {
    albedo = glm::vec3(0.1f);
    intensity = glm::vec3(0);
    is_light = false;
    transform = glm::mat4(1.0f);
    inv_transform = glm::mat4(1.0f);
    inv_transpose_transform = glm::mat3(1.0f);
  }

  virtual float intersect(const glm::vec3 &o, const glm::vec3 &d,
                          const float t_min, const float t_max,
                          glm::vec3 &hit_normal) const = 0;

  virtual glm::vec3 get_position() const { throw "Not implemented"; }

  Material *material = new Diffuse();
  glm::vec3 albedo;
  glm::mat4 transform, inv_transform;
  glm::mat3 inv_transpose_transform;

  // color of the light
  glm::vec3 intensity;

  virtual glm::vec3 get_le(UNUSED glm::vec3 ray_origin,
                           UNUSED glm::vec3 ray_intersection) const {
    return intensity;
  }

  // setters
  void set_albedo(const glm::vec3 &albedo) { this->albedo = albedo; }
  void set_intensity(const glm::vec3 &intensity) {
    this->intensity = intensity;
    this->is_light = glm::length(intensity) > 0;
  }
  void set_transform(const glm::mat4 &transform);
  void set_material(Material *material) { this->material = material; }
  bool is_light_source() const { return is_light; }

  void get_transforms(glm::vec3 &v, glm::vec3 &p) const;
  void normal_transform(glm::vec3 &n) const;

  virtual ~Shape() { delete material; }
};

class Sphere : public Shape {
public:
  glm::vec3 get_position() const override;
  float intersect(const glm::vec3 &o, const glm::vec3 &d, const float t_min,
                  const float t_max, glm::vec3 &hit_normal) const override;

  // setters
  void set_radius(const float radius) { this->radius = radius; }
  void set_center(const glm::vec3 &center) { this->center = center; }

private:
  glm::vec3 center;
  float radius;
};

class Plane : public Shape {
public:
  float intersect(const glm::vec3 &o, const glm::vec3 &d, const float t_min,
                  const float t_max, glm::vec3 &hit_normal) const override;

  // setters
  void set_normal(const glm::vec3 &normal) { this->normal = normal; }
  void set_d(const float d) { this->d = d; }

private:
  glm::vec3 normal;
  float d;
};

class Cuboid : public Shape {
public:
  float intersect(const glm::vec3 &o, const glm::vec3 &d, const float t_min,
                  const float t_max, glm::vec3 &hit_normal) const override;

  // setters
  void set_bounds(const glm::vec3 &min, const glm::vec3 &max) {
    this->min_coords = min;
    this->max_coords = max;
    check_coordinates();
  }

private:
  glm::vec3 min_coords;
  glm::vec3 max_coords;

  void check_coordinates() {
    assert(min_coords.x <= max_coords.x);
    assert(min_coords.y <= max_coords.y);
    assert(min_coords.z <= max_coords.z);
  }
};

class Point : public Shape {
public:
  float intersect(const glm::vec3 &o, const glm::vec3 &d, const float t_min,
                  const float t_max, glm::vec3 &hit_normal) const override;
  glm::vec3 get_position() const override;
  glm::vec3 get_le(glm::vec3 ray_origin,
                   glm::vec3 ray_intersection) const override;

  // setters
  void set_position(const glm::vec3 &position) { this->position = position; }

private:
  glm::vec3 position;
};
