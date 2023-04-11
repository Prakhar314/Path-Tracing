#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "shape.hpp"
using namespace std;

#define PI 3.14159265358979323846f

class RayTracer {
public:
  RayTracer(const int width, const int height, const float vfov,
            const glm::vec3 &camera_position, const glm::vec3 &camera_direction,
            const glm::vec3 &camera_up, const bool path_tracing = false);
  glm::uvec3 **render(const std::vector<Shape *> &shapes, int num_samples = 16);

private:
  const float T_MIN = 0.001;
  const float T_MAX = 1000;
  const int EXP_BOUNCES = 5;
  const bool path_tracing;
  const float width, height, vfov, hfov;
  const glm::vec3 camera_position, camera_direction, camera_up;

  glm::uvec3 **framebuffer;
  glm::vec3 **path_tracing_framebuffer;
  int path_tracing_count = 0;

  void shoot_ray(const glm::vec3 &o, const glm::vec3 &d,
                 const std::vector<Shape *> &shapes, float &t,
                 glm::vec3 &normal, Shape *&shape) const;
  bool shadow(const glm::vec3 &o, const Shape *light,
              const std::vector<Shape *> &shapes) const;
  glm::vec3 trace(const glm::vec3 &o, const glm::vec3 &d,
                  const std::vector<Shape *> &shape, int recursion_depth) const;
};
