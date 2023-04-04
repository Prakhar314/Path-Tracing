#include "shape.hpp"
#include <iostream>

glm::vec3 Point::get_position() const{
  return position;
}

float Point::intersect(const glm::vec3 &o, const glm::vec3 &d, const float t_min, const float t_max, glm::vec3 &hit_normal) const{
  float t = glm::dot(position - o, d);
  if (t < t_min || t > t_max) {
    return -1.0f;
  }
  if (glm::length(position - o - t * d) > 0.001f) {
    return -1.0f;
  }
  hit_normal = glm::normalize(position - o);
  return t;
}

glm::vec3 Sphere::get_position() const{
  return center;
}

float Sphere::intersect(const glm::vec3& o, const glm::vec3& d, const float t_min, const float t_max, glm::vec3& hit_normal) const{
  // let the point on the sphere be p = o + td
  // then the equation of the sphere is (p - c) * (p - c) = r^2
  // (o + td - c) * (o + td - c) = r^2
  // (o - c) * (o - c) + 2td * (o - c) + t^2d^2 = r^2
  // t^2d^2 + 2td * (o - c) + (o - c) * (o - c) - r^2 = 0
  float a = glm::dot(d, d);
  float b = 2 * glm::dot(d, o - center);
  float c = glm::dot(o - center, o - center) - radius * radius;
  // then the discriminant is b^2 - 4ac
  float discriminant = b * b - 4 * a * c;
  // if the discriminant is negative, there is no intersection
  if (discriminant < 0) {
    return -1.0f;
  }
  float t = 0;
  // if the discriminant is zero, there is one intersection
  if (discriminant == 0) {
    t = -b / (2 * a);
    if (t < t_min || t > t_max) {
      return 1.0f;
    }
  }
  // if the discriminant is positive, there are two intersections
  else{
    float t1 = (-b + sqrt(discriminant)) / (2 * a);
    float t2 = (-b - sqrt(discriminant)) / (2 * a);
    // we want the smaller positive t
    t = t1 < t2 ? t1 : t2;
    if(t < t_min || t > t_max){
      t = t1 > t2 ? t1 : t2;
    }
    if (t < t_min || t > t_max) {
      return -1.0f;
    }
  }
  assert(t >= t_min && t <= t_max && t > 0);
  glm::vec3 p = o + t * d;
  // hit inside or outside
  if (glm::dot(p - center, d) < 0) {
    hit_normal = glm::normalize(p - center);
  }
  else {
    hit_normal = glm::normalize(center - p);
  }
  return t;
}

float Plane::intersect(const glm::vec3 &o, const glm::vec3 &d, const float t_min, const float t_max, glm::vec3 &hit_normal) const{
  // let the point on the plane be p = o + td
  // then the equation of the plane is n * p = d
  // n * (o + td) = d
  // n * o + n * td = d
  // t = (d - n * o) / (n * d)
  if (glm::dot(normal, d) == 0) {
    return -1.0f;
  }
  float t = (this->d - glm::dot(normal, o)) / glm::dot(normal, d);
  if (t < t_min || t > t_max) {
    return -1.0f;
  }
  // hit above or below
  if (glm::dot(normal, d) > 0) {
    hit_normal = -normal;
  }
  else {
    hit_normal = normal;
  }
  return t;
}

float Cuboid::intersect(const glm::vec3 &o, const glm::vec3 &d, const float t_min, const float t_max, glm::vec3& hit_normal) const{
  // let the point on the cuboid be p = o + td
  // then the equation of the cuboid is min.x <= p.x <= max.x and so on
  float t_x_min = (min_coords.x - o.x) / d.x;
  float t_x_max = (max_coords.x - o.x) / d.x;
  // swap if necessary
  if (t_x_min > t_x_max) {
    float temp = t_x_min;
    t_x_min = t_x_max;
    t_x_max = temp;
  }
  float t_y_min = (min_coords.y - o.y) / d.y;
  float t_y_max = (max_coords.y - o.y) / d.y;
  if (t_y_min > t_y_max) {
    float temp = t_y_min;
    t_y_min = t_y_max;
    t_y_max = temp;
  }
  float t_z_min = (min_coords.z - o.z) / d.z;
  float t_z_max = (max_coords.z - o.z) / d.z;
  if (t_z_min > t_z_max) {
    float temp = t_z_min;
    t_z_min = t_z_max;
    t_z_max = temp;
  }
  float t_in_min = fmax(t_x_min, fmax(t_y_min, t_z_min));
  float t_in_max = fmin(t_x_max, fmin(t_y_max, t_z_max));
  if (t_in_min > t_in_max) {
    return -1.0f;
  }
  if (t_in_min < t_min){
    if (t_in_max > t_max || t_in_max < t_min) {
      return -1.0f;
    }
    if (t_in_max == t_x_max) {
      hit_normal = glm::vec3(d.x < 0 ? 1 : -1, 0, 0);
    }
    else if (t_in_max == t_y_max) {
      hit_normal = glm::vec3(0, d.y < 0 ? 1 : -1, 0);
    }
    else{
      hit_normal = glm::vec3(0, 0, d.z < 0 ? 1 : -1);
    }
    return t_in_max;
  }
  else{
    if (t_in_min > t_max || t_in_min < t_min) {
      return -1.0f;
    }
    if (t_in_min == t_x_min) {
      hit_normal = glm::vec3(d.x < 0 ? 1 : -1, 0, 0);
    }
    else if (t_in_min == t_y_min) {
      hit_normal = glm::vec3(0, d.y < 0 ? 1 : -1, 0);
    }
    else{
      hit_normal = glm::vec3(0, 0, d.z < 0 ? 1 : -1);
    }
    return t_in_min;
  }
}


