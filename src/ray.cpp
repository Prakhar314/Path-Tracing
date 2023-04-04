#include "ray.hpp"
#include <iostream>

#define PI 3.14159265358979323846

RayTracer::RayTracer(const int width, const int height, const float vfov, const glm::vec3& camera_position, const glm::vec3& camera_direction, const glm::vec3& camera_up):
  width(width), height(height), 
  vfov(vfov * PI / 180), 
  hfov(2 * atan(tan(vfov / 2 * PI / 180) * width / height)), 
  camera_position(camera_position), camera_direction(camera_direction), camera_up(camera_up)
{
  framebuffer = new glm::uvec3*[width];
  for (int i = 0; i < width; i++) {
    framebuffer[i] = new glm::uvec3[height];
  }
}

glm::uvec3** RayTracer::render(const std::vector<Shape*>& shapes, const std::vector<LightSource*>& lights) {
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      float u = i / (width - 1);
      float v = j / (height - 1);
      glm::vec3 vertical = camera_up;
      glm::vec3 horizontal = glm::normalize(glm::cross(camera_direction, vertical));
      //float v_w = tan((v - 0.5f) * vfov);
      //float h_w = tan((u - 0.5f) * hfov);
      //assert(v_w >= -tan(vfov / 2) && v_w <= tan(vfov / 2));
      //assert(h_w >= -tan(hfov / 2) && h_w <= tan(hfov / 2));
      float v_w = (2 * v - 1) * tan(vfov / 2);
      float h_w = (2 * u - 1) * tan(hfov / 2);
      glm::vec3 d = glm::normalize(camera_direction + h_w * horizontal + v_w * vertical);
      framebuffer[i][j] = glm::uvec3(255.0f * trace(camera_position, d, shapes, lights));
    }
  }
  return framebuffer;
}

glm::vec3 RayTracer::trace(const glm::vec3& o, const glm::vec3& d, const std::vector<Shape*>& shapes, const std::vector<LightSource*>& lights){
  // find the closest shape and its intersection point
  Shape* closest_shape = nullptr;
  glm::vec3 closest_position, closest_normal;
  float closest_t = std::numeric_limits<float>::max();
  for(Shape* shape : shapes){
    glm::vec3 temp_position, temp_normal;
    float t = shape->intersect(o, d, T_MIN, T_MAX, temp_position, temp_normal);
    if(t > T_MIN && t < closest_t){
      closest_t = t;
      closest_shape = shape;
      closest_position = temp_position;
      closest_normal = temp_normal;
    }
  }
  // if there is no intersection, return the background color
  if(closest_shape == nullptr){
    return glm::vec3(0.0f);
  }
  // if there are no lights, use the default scheme
  if(lights.size() == 0){
    return (closest_normal + glm::vec3(1.0f)) / 2.0f;
  }
  else{
    return glm::vec3(1.0f);
  }
}
