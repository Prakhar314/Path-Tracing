#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "shape.hpp"
using namespace std;

class RayTracer {
public:
  RayTracer(const int width, const int height, const float vfov, const glm::vec3& camera_position, const glm::vec3& camera_direction, const glm::vec3& camera_up);
  glm::uvec3** render(const std::vector<Shape*>& shapes);
private:
  const float T_MIN = 0.001;
  const float T_MAX = 1000;
  const float width, height, vfov, hfov;
  const glm::vec3 camera_position, camera_direction, camera_up;

  glm::uvec3** framebuffer;
  
  void shoot_ray(const glm::vec3& o, const glm::vec3& d, const std::vector<Shape*>& shapes, float& t, glm::vec3& normal, Shape*& shape) const;
  bool shadow(const glm::vec3& o, const Shape* light, const std::vector<Shape*>& shapes) const;
  glm::vec3 SampleDiffuse(const glm::vec3& normal) const;
  glm::vec3 trace(const glm::vec3& o, const glm::vec3& d, const std::vector<Shape*>& shapes, const int rd) const;
};
