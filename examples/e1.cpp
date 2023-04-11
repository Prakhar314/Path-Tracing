#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/fwd.hpp>

int main() {
  RayTracer raytracer(640, 480, 90, glm::vec3(0), glm::vec3(0, 0, -1),
                      glm::vec3(0, 1, 0));
  Viewer viewer(640, 480, "Raytracer");
  vector<Shape *> shapes;
  shapes.push_back(new Sphere(glm::vec3(0, 0, -2), 1.0f, glm::vec3(1)));
  shapes.push_back(new Sphere(glm::vec3(0, -101, -2), 100.0f, glm::vec3(1)));
  glm::uvec3 **output = raytracer.render(shapes);
  while (!viewer.shouldQuit()) {
    viewer.update(output);
  }
  return 0;
}
