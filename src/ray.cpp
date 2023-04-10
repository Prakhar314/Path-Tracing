#include "ray.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <math.h>
#include <random>

using namespace std;

#define PI 3.14159265358979323846f

float rand_f() { return static_cast<double>(rand()) / RAND_MAX; }

RayTracer::RayTracer(const int width, const int height, const float vfov,
                     const glm::vec3 &camera_position,
                     const glm::vec3 &camera_direction,
                     const glm::vec3 &camera_up, bool path_tracing)
    : width(width), height(height), vfov(vfov * PI / 180),
      hfov(2 * atan(tan(vfov / 2 * PI / 180) * width / height)),
      camera_position(camera_position), camera_direction(camera_direction),
      camera_up(camera_up), path_tracing(path_tracing) {
  framebuffer = new glm::uvec3 *[width];
  for (int i = 0; i < width; i++) {
    framebuffer[i] = new glm::uvec3[height];
    for (int j = 0; j < height; j++) {
      framebuffer[i][j] = glm::uvec3(0);
    }
  }
  if (path_tracing) {
    path_tracing_framebuffer = new glm::vec3 *[width];
    for (int i = 0; i < width; i++) {
      path_tracing_framebuffer[i] = new glm::vec3[height];
      for (int j = 0; j < height; j++) {
        path_tracing_framebuffer[i][j] = glm::vec3(0);
      }
    }
  }
}

glm::uvec3 **RayTracer::render(const std::vector<Shape *> &shapes,
                               int num_samples) {
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      glm::vec3 color_sum(0.0f);
      if (path_tracing) {
        color_sum = path_tracing_framebuffer[i][j];
      }

      int sqrt_s = sqrt(num_samples);

      for (int k = 0; k < num_samples; k++) {
        float du = 0.0f;
        float dv = 0.0f;
        if (!path_tracing && sqrt_s > 1) {
          int a = k / sqrt_s;
          int b = k % sqrt_s;
          du = static_cast<float>(a) / (sqrt_s - 1);
          dv = static_cast<float>(b) / (sqrt_s - 1);
        }
        float u = (i + du) / (width - 1);
        float v = (j + dv) / (height - 1);
        // std::cout<<du<<" "<<dv<<"\n";
        glm::vec3 vertical = camera_up;
        glm::vec3 horizontal =
            glm::normalize(glm::cross(camera_direction, vertical));
        float v_w = (2 * v - 1) * tan(vfov / 2);
        float h_w = (2 * u - 1) * tan(hfov / 2);
        glm::vec3 d = glm::normalize(camera_direction + h_w * horizontal +
                                     v_w * vertical);
        color_sum += trace(camera_position, d, shapes, 0) /
                     static_cast<float>(num_samples);
      }
      if (path_tracing) {
        path_tracing_framebuffer[i][j] = color_sum;
        color_sum /= (1 + this->path_tracing_count);
      }
      color_sum /= (1.0f + color_sum);
      // gamma correction
      for (int k = 0; k < 3; k++) {
        if (color_sum[k] <= 0.0031308) {
          color_sum[k] *= 12.92;
        } else {
          color_sum[k] = 1.055 * pow(color_sum[k], 1 / 2.4) - 0.055;
        }
      }
      framebuffer[i][j] = glm::uvec3(255.0f * color_sum);
    }
  }
  path_tracing_count++;
  return framebuffer;
}

void RayTracer::shoot_ray(const glm::vec3 &o, const glm::vec3 &d,
                          const std::vector<Shape *> &shapes, float &hit_t,
                          glm::vec3 &hit_normal, Shape *&hit_shape) const {
  // find the closest shape and its intersection point
  Shape *closest_shape = nullptr;
  glm::vec3 closest_position, closest_normal;
  float closest_t = std::numeric_limits<float>::max();

  // intersect with all shapes
  for (Shape *shape : shapes) {
    glm::vec3 temp_normal;
    float t = shape->intersect(o, d, T_MIN, T_MAX, temp_normal);
    // std::cout<<"t: "<<t<<"\n";
    if (t >= T_MIN && t < closest_t && t <= T_MAX) {
      closest_t = t;
      closest_shape = shape;
      closest_normal = temp_normal;
    }
  }

  if (closest_shape != nullptr) {
    hit_t = closest_t;
    hit_normal = closest_normal;
    hit_shape = closest_shape;
  } else {
    hit_shape = nullptr;
  }
}

bool RayTracer::shadow(const glm::vec3 &o, const Shape *light,
                       const std::vector<Shape *> &shapes) const {
  // check if the ray from o to light->position intersects any shape
  glm::vec3 d = glm::normalize(light->get_position() - o);
  // using shoot_ray
  float closest_t;
  glm::vec3 closest_normal;
  Shape *closest_shape;
  shoot_ray(o, d, shapes, closest_t, closest_normal, closest_shape);
  if (closest_shape != nullptr) {
    if (closest_shape != light) {
      return true;
    }
  }
  return false;
}

glm::vec3 RayTracer::trace(const glm::vec3 &o, const glm::vec3 &d,
                           const std::vector<Shape *> &shapes,
                           int recursion_depth) const {
  // find the closest shape and its intersection point using shoot_ray
  float closest_t;
  glm::vec3 closest_normal;
  Shape *closest_shape;
  shoot_ray(o, d, shapes, closest_t, closest_normal, closest_shape);
  glm::vec3 closest_position = o + closest_t * d;
  // if there is no intersection, return the background color
  if (closest_shape == nullptr) {
    return glm::vec3(0.5f); // sky color
  }
  if (path_tracing) {
    // russian roulette
    if (rand_f() > 1.0f - 1.0f / EXP_BOUNCES) {
      return glm::vec3(0.0f);
    }
  } else if (recursion_depth >= EXP_BOUNCES * 2) {
    return glm::vec3(0.5f);
  }

  bool is_light = closest_shape->is_light;
  int light_count = 0;
  if (is_light) {
    return closest_shape->get_le(o, closest_position);
  } else {
    for (Shape *shape : shapes) {
      if (shape->is_light) {
        light_count++;
        break;
      }
    }
  }

  // if there are no lights, use the default scheme
  if (light_count == 0) {
    return (closest_normal + glm::vec3(1.0f)) / 2.0f;
  } else {
    // Shape is metallic
    if (closest_shape->material->is_metallic) {
      glm::vec3 l_i(0.0f);
      glm::vec3 reflection_dir =
          closest_shape->material->reflect(closest_normal, d);
      glm::vec3 reflectance =
          closest_shape->material->reflectance(closest_normal, d);

      l_i += reflectance * trace(closest_position, reflection_dir, shapes,
                                 recursion_depth + 1); // with reflected ray
      return l_i;
      // Shape is transparent
    } else if (closest_shape->material->is_transparent) {
      // Reflect and refract
      glm::vec3 l_i(0.0f);
      glm::vec3 reflectans =
          closest_shape->material->reflectance(closest_normal, d);
      glm::vec3 reflected_dir =
          closest_shape->material->reflect(closest_normal, d);
      glm::vec3 transmittans = glm::vec3(1.0f) - reflectans;
      glm::vec3 transmitted_dir =
          closest_shape->material->transmit(closest_normal, d);
      // fresnel weighted sampling
      float e_1 = rand_f();
      if (!path_tracing || e_1 <= reflectans[0]) {
        l_i += reflectans * trace(closest_position, reflected_dir, shapes,
                                  recursion_depth + 1); // with reflected ray
      }
      if (!path_tracing || e_1 > reflectans[0]) {

        l_i += transmittans * trace(closest_position, transmitted_dir, shapes,
                                    recursion_depth + 1); // with refracted ray
      }
      return l_i;
      // Shape is diffuse
    } else {
      glm::vec3 l_i(0.0f);
      if (path_tracing) {
        // cosine weighted sampling
        float e_1 = rand_f();
        float e_2 = rand_f();
        float phi = 2 * PI * e_1;
        float cos_theta = sqrt(e_2);
        float sin_theta = sqrt(1 - e_2);

        glm::vec3 n = closest_normal;
        glm::vec3 u = glm::normalize(glm::cross(n, d));
        glm::vec3 v = glm::normalize(glm::cross(u, n));

        glm::vec3 sample_dir = glm::normalize(
            cos_theta * n + sin_theta * (cos(phi) * u + sin(phi) * v));

        l_i += closest_shape->albedo * cos_theta *
               trace(closest_position, sample_dir, shapes, recursion_depth + 1);

      } else {
        for (Shape *shape : shapes) {
          if (shape->is_light) {
            glm::vec3 l = shape->get_position();
            if (!shadow(closest_position, shape, shapes)) {
              glm::vec3 l_dir = glm::normalize(l - closest_position);
              float cos_theta = abs(glm::dot(closest_normal, l_dir));
              l_i += closest_shape->albedo * cos_theta *
                     shape->get_le(closest_position, l);
            }
          }
        }
      }

      return l_i;
    }
  }
}
