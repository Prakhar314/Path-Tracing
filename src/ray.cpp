#include "ray.hpp"
#include <iostream>

#define PI 3.14159265358979323846f

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

glm::uvec3** RayTracer::render(const std::vector<Shape*>& shapes) {
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
      framebuffer[i][j] = glm::uvec3(255.0f * trace(camera_position, d, shapes));
    }
  }
  // normalize the framebuffer if any value is greater than 255
  uint32_t max = 0;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      if (framebuffer[i][j].x > max) max = framebuffer[i][j].x;
      if (framebuffer[i][j].y > max) max = framebuffer[i][j].y;
      if (framebuffer[i][j].z > max) max = framebuffer[i][j].z;
    }
  }
  if (max > 255) {
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
        framebuffer[i][j] = framebuffer[i][j] * 255u / max;
      }
    }
  }
  return framebuffer;
}

void RayTracer::shoot_ray(const glm::vec3& o, const glm::vec3& d, const std::vector<Shape*>& shapes, float& hit_t, glm::vec3& hit_normal, Shape*& hit_shape) const{
  // find the closest shape and its intersection point
  Shape* closest_shape = nullptr;
  glm::vec3 closest_position, closest_normal;
  float closest_t = std::numeric_limits<float>::max();
  
  // intersect with all shapes
  for(Shape* shape : shapes){
    glm::vec3 temp_normal;
    float t = shape->intersect(o, d, T_MIN, T_MAX, temp_normal);
    if(t > T_MIN && t < closest_t && t < T_MAX){
      closest_t = t;
      closest_shape = shape;
      closest_normal = temp_normal;
    }
  }

  if (closest_shape != nullptr) {
    hit_t = closest_t;
    hit_normal = closest_normal;
    hit_shape = closest_shape;
  }
  else{
    hit_shape = nullptr;
  }
}

bool RayTracer::shadow(const glm::vec3& o, const Shape* light, const std::vector<Shape*>& shapes) const{
  // check if the ray from o to light->position intersects any shape
  glm::vec3 d = glm::normalize(light->get_position() - o);
  // using shoot_ray
  float closest_t;
  glm::vec3 closest_normal;
  Shape* closest_shape;
  shoot_ray(o, d, shapes, closest_t, closest_normal, closest_shape);
  if(closest_shape != nullptr){
    if(closest_shape != light){
      return true;
    }
  }
  return false;
}

glm::vec3 RayTracer::trace(const glm::vec3& o, const glm::vec3& d, const std::vector<Shape*>& shapes) const{
  // find the closest shape and its intersection point using shoot_ray
  float closest_t;
  glm::vec3 closest_normal;
  Shape* closest_shape;
  shoot_ray(o, d, shapes, closest_t, closest_normal, closest_shape);
  glm::vec3 closest_position = o + closest_t * d;
  // if there is no intersection, return the background color
  if(closest_shape == nullptr){
    return glm::vec3(0.0f);
  }

  vector<Shape*> lights;
  for(Shape* shape : shapes){
    if(shape->is_light){
      lights.push_back(shape);
    }
  }

  // if there are no lights, use the default scheme
  if(lights.size() == 0){
    return (closest_normal + glm::vec3(1.0f)) / 2.0f;
  }
  else{
    // check if the shape is a light
    for(Shape* light : lights){
      if(closest_shape == light){
        glm::vec3 l = light->get_position() - closest_position;
        return light->intensity / glm::dot(l, l);
      }
    }
    // not a light
    glm::vec3 l_i(0.0f);
    for(Shape* light : lights){
      glm::vec3 l = light->get_position() - closest_position;
      float distance = glm::length(l);
      l = l / distance;
      if(!shadow(closest_position, light, shapes)){
        // flux = I * dA cos(theta) / r^2
        // irradiance = flux / dA
        // radiance = brdf * irradiance
        glm::vec3 e = light->intensity * glm::max(glm::dot(l, closest_normal), 0.0f) / (distance * distance);
        l_i += closest_shape->albedo / PI * e; 
      }
    }
    return l_i;
  }
}
