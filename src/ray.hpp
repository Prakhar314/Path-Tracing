#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "shape.hpp"

using namespace std;

struct LightSource {
  glm::vec3 position;
  glm::vec3 radiance;
};

class RayTracer {
public:
  RayTracer(const int width, const int height, const float vfov, const glm::vec3& camera_position, const glm::vec3& camera_direction, const glm::vec3& camera_up);
  glm::uvec3** render(const std::vector<Shape*>& shapes, const std::vector<LightSource*>& lights);
private:
  const float T_MIN = 0.001;
  const float T_MAX = 1000;
  const float width, height, vfov, hfov;
  const glm::vec3 camera_position, camera_direction, camera_up;

  glm::uvec3** framebuffer;
  glm::vec3 trace(const glm::vec3& o, const glm::vec3& d, const std::vector<Shape*>& shapes, const std::vector<LightSource*>& lights);
  bool shadow(const glm::vec3& o, const glm::vec3& d, const std::vector<LightSource*>& lights);
};
