#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "shape.hpp"
using namespace std;

#define PI 3.14159265358979323846f

class RayTracer {
public:
  glm::uvec3 **render(const std::vector<Shape *> &shapes, int num_samples = 16,
                      const bool gamma_correction = true);

  // setters
  void set_resolution(const int width, const int height) {
    this->width = width;
    this->height = height;
  }
  void set_fov(const float vfov) {
    this->vfov = vfov * PI / 180;
    this->hfov = 2 * atan(tan(this->vfov / 2) * width / height);
  }
  void set_camera(const glm::vec3 &camera_position,
                  const glm::vec3 &camera_direction,
                  const glm::vec3 &camera_up) {
    this->camera_position = camera_position;
    this->camera_direction = camera_direction;
    this->camera_up = camera_up;
  }
  void set_path_tracing(const bool path_tracing) {
    this->path_tracing = path_tracing;
  }
  void init();

  ~RayTracer() {
    if (framebuffer != nullptr) {
      for (int i = 0; i < width; i++) {
        delete[] framebuffer[i];
        framebuffer[i] = nullptr;
      }
    }
    if (path_tracing_framebuffer != nullptr) {
      for (int i = 0; i < width; i++) {
        delete[] path_tracing_framebuffer[i];
        path_tracing_framebuffer[i] = nullptr;
      }
    }
    delete[] framebuffer;
    delete[] path_tracing_framebuffer;
  }

private:
  const float T_MIN = 0.001;
  const float T_MAX = 1000;
  const int EXP_BOUNCES = 5;

  bool path_tracing;
  int width, height;
  float vfov, hfov;
  glm::vec3 camera_position, camera_direction, camera_up;

  glm::uvec3 **framebuffer = nullptr;
  glm::vec3 **path_tracing_framebuffer = nullptr;
  int path_tracing_count = 0;

  void shoot_ray(const glm::vec3 &o, const glm::vec3 &d,
                 const std::vector<Shape *> &shapes, float &t,
                 glm::vec3 &normal, Shape *&shape) const;
  bool shadow(const glm::vec3 &o, const Shape *light,
              const std::vector<Shape *> &shapes) const;
  glm::vec3 trace(const glm::vec3 &o, const glm::vec3 &d,
                  const std::vector<Shape *> &shape, int recursion_depth) const;
};
