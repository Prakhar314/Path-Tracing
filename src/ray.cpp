#include "ray.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <math.h>
#include <random>
#include <thread>

using namespace std;

float rand_f() {
  unsigned int lo, hi;

  // RDTSC copies contents of 64-bit TSC into EDX:EAX
  asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
  return static_cast<double>(rand_r(&lo)) / RAND_MAX;
}

void RayTracer::init(const std::vector<Shape *> &shapes, const int num_samples,
                     const bool gamma_correction) {

  this->shapes = &shapes;
  this->num_samples = num_samples;
  this->gamma_correction = gamma_correction;
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
  if (!thread_lock.owns_lock()) {
    thread_lock.lock();
  }
  if (num_threads > 1) {
    for (int i = 1; i < num_threads; i++) {
      auto forever = [this](int thread_id) {
        while (true) {
          uint32_t time = this->render_thread(thread_id);
          unique_lock<mutex> lk(thread_mutex);
          // int start = thread_id * width / num_threads;
          // int end = (thread_id + 1) * width / num_threads;
          // cout << "thread " << thread_id << " finished in " << time << "ms\n";
          // cout << start << " " << end << "\n";
          this->thread_sync++;
          if (this->thread_sync == this->num_threads) {
            cv_m.notify_all();
          }
          int old_count = this->path_tracing_count;
          while (old_count == this->path_tracing_count) {
            cv_t.wait(lk);
          }
        }
      };
      thread_pool.push_back(thread(forever, i));
    }
  }
}

uint32_t RayTracer::render_thread(int thread_id) {
  uint32_t start = std::chrono::duration_cast<std::chrono::milliseconds>(
                       std::chrono::system_clock::now().time_since_epoch())
                       .count();
  for (int i = thread_id * width / num_threads;
       i < (thread_id + 1) * width / num_threads && i < width; i++) {
    for (int j = 0; j < height; j++) {
      glm::vec3 color_sum(0.0f);
      if (path_tracing) {
        color_sum = path_tracing_framebuffer[i][j];
      }

      int sqrt_s = sqrt(num_samples);

      for (int k = 0; k < num_samples; k++) {
        float du = 0.0f;
        float dv = 0.0f;
        if (sqrt_s > 1) {
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
        color_sum +=
            trace(camera_position, d, 0) / static_cast<float>(num_samples);
      }
      if (path_tracing) {
        path_tracing_framebuffer[i][j] = color_sum;
        color_sum /= (1 + this->path_tracing_count);
      }
      color_sum /= (1.0f + color_sum);
      if (gamma_correction) {
        for (int k = 0; k < 3; k++) {
          if (color_sum[k] <= 0.0031308) {
            color_sum[k] *= 12.92;
          } else {
            color_sum[k] = 1.055 * pow(color_sum[k], 1 / 2.4) - 0.055;
          }
        }
      }
      framebuffer[i][j] = glm::uvec3(255.0f * color_sum);
    }
  }
  uint32_t end = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                     .count();

  return end - start;
}

glm::uvec3 **RayTracer::render() {
  thread_lock.unlock();
  uint32_t time = render_thread(0);
  thread_lock.lock();
  // cout << "thread " << 0 << " finished in " << time << "ms\n";
  thread_sync++;
  while (thread_sync < num_threads) {
    cv_m.wait(thread_lock);
  }
  thread_sync = 0;
  path_tracing_count++;
  cv_t.notify_all();
  std::cout << "path tracing count: " << path_tracing_count << std::endl;
  return framebuffer;
}

void RayTracer::shoot_ray(const glm::vec3 &o, const glm::vec3 &d, float &hit_t,
                          glm::vec3 &hit_normal, Shape *&hit_shape) const {
  // find the closest shape and its intersection point
  hit_t = std::numeric_limits<float>::max();
  hit_shape = nullptr;

  // intersect with all shapes
  for (Shape *shape : *shapes) {
    glm::vec3 temp_normal;
    float t = shape->intersect(o, d, T_MIN, T_MAX, temp_normal);
    if (t >= T_MIN && t < hit_t && t <= T_MAX) {
      hit_t = t;
      hit_shape = shape;
      hit_normal = temp_normal;
    }
  }
  if (hit_shape == nullptr) {
    hit_t = -1;
  }
}

bool RayTracer::shadow(const glm::vec3 &o, const Shape *light) const {
  // check if the ray from o to light->position intersects any shape
  glm::vec3 d = glm::normalize(light->get_position() - o);
  // using shoot_ray
  float closest_t;
  glm::vec3 closest_normal;
  Shape *closest_shape;
  shoot_ray(o, d, closest_t, closest_normal, closest_shape);
  if (closest_shape != nullptr) {
    if (closest_shape != light) {
      return true;
    }
  }
  return false;
}

glm::vec3 RayTracer::trace(const glm::vec3 &o, const glm::vec3 &d,
                           int recursion_depth) const {
  // find the closest shape and its intersection point using shoot_ray
  float closest_t;
  glm::vec3 closest_normal;
  Shape *closest_shape;
  shoot_ray(o, d, closest_t, closest_normal, closest_shape);
  glm::vec3 closest_position = o + closest_t * d;
  // if there is no intersection, return the background color
  if (closest_shape == nullptr) {
    return glm::vec3(0.0f); // sky color
  }

  const float continue_prob = 1.0f - 1.0f / EXP_BOUNCES;

  if (path_tracing) {
    // russian roulette
    if (rand_f() > continue_prob) {
      return glm::vec3(0.0f);
    }
  } else if (recursion_depth >= EXP_BOUNCES * 2) {
    return glm::vec3(0.0f);
  }

  bool is_light = closest_shape->is_light_source();
  int light_count = 0;
  if (is_light) {
    return closest_shape->get_le(o, closest_position);
  } else {
    for (Shape *shape : *shapes) {
      if (shape->is_light_source()) {
        light_count++;
        break;
      }
    }
  }

  glm::vec3 l_i(0.0f);
  // if there are no lights, use the default scheme
  if (light_count == 0) {
    return (closest_normal + glm::vec3(1.0f)) / 2.0f;
  } else {
    // Shape is metallic
    if (closest_shape->material->isMetallic()) {
      glm::vec3 reflected_dir =
          closest_shape->material->reflect(closest_normal, d);
      glm::vec3 reflectance =
          closest_shape->material->reflectance(closest_normal, d);

      l_i += reflectance *
             trace(closest_position, reflected_dir, recursion_depth + 1);
      // Shape is transparent
    } else if (closest_shape->material->isTransparent()) {
      // Reflect and refract
      glm::vec3 reflectance =
          closest_shape->material->reflectance(closest_normal, d);
      glm::vec3 reflected_dir =
          closest_shape->material->reflect(closest_normal, d);
      glm::vec3 transmittance = glm::vec3(1.0f) - reflectance;
      glm::vec3 transmitted_dir =
          closest_shape->material->transmit(closest_normal, d);
      // fresnel weighted sampling
      float e_1 = rand_f();
      if (!path_tracing || e_1 <= reflectance[0]) {
        if (path_tracing) {
          l_i += trace(closest_position, reflected_dir, recursion_depth + 1);
        } else {
          l_i += reflectance *
                 trace(closest_position, reflected_dir, recursion_depth + 1);
        }
      }
      if (!path_tracing || e_1 > reflectance[0]) {
        if (path_tracing) {
          l_i += trace(closest_position, transmitted_dir, recursion_depth + 1);
        } else {
          l_i += transmittance *
                 trace(closest_position, transmitted_dir, recursion_depth + 1);
        }
      }
      // Shape is diffuse
    } else {
      if (path_tracing) {
        // cosine weighted sampling
        float e_1 = rand_f();
        float e_2 = rand_f();
        float phi = 2 * PI * e_1;
        float cos_theta = sqrt(e_2);
        float sin_theta = sqrt(1 - e_2);

        glm::vec3 n = closest_normal;
        if (glm::dot(n, d) > 0) {
          n = -n;
        }
        glm::vec3 u = glm::normalize(glm::cross(n, d));
        glm::vec3 v = glm::normalize(glm::cross(u, n));

        glm::vec3 sample_dir = glm::normalize(
            cos_theta * n + sin_theta * (cos(phi) * u + sin(phi) * v));

        l_i += closest_shape->albedo *
               trace(closest_position, sample_dir, recursion_depth + 1);

      } else {
        // sample all light sources
        for (Shape *shape : *shapes) {
          if (shape->is_light_source()) {
            glm::vec3 l = shape->get_position();
            if (!shadow(closest_position, shape)) {
              glm::vec3 l_dir = glm::normalize(l - closest_position);
              float cos_theta = abs(glm::dot(closest_normal, l_dir));
              l_i += closest_shape->albedo * cos_theta / PI *
                     shape->get_le(closest_position, l);
            }
          }
        }
      }
    }
    if (path_tracing) {
      l_i /= continue_prob;
    }
    return l_i;
  }
}
