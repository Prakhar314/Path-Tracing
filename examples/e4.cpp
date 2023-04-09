#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/fwd.hpp>
// example with only diffuse objects
int main() {
  RayTracer raytracer(640, 480, 90, glm::vec3(0), glm::vec3(0, 0, -1),
                      glm::vec3(0, 1, 0));
  Viewer viewer(640, 480, "Raytracer");
  vector<Shape *> shapes;
  shapes.push_back(new Sphere(glm::vec3(0, 0, -2), 1.0f, glm::vec3(1, 0, 0)));
  shapes.push_back(new Plane(glm::vec3(0, 1, 0), -1, glm::vec3(0, 1, 1)));
  shapes.push_back(
      new Cuboid(glm::vec3(2, 1, -4), glm::vec3(3, 2, -3), glm::vec3(0, 0, 1)));
  shapes.push_back(new Cuboid(glm::vec3(-3, -1, -4), glm::vec3(-2, -0.5, -3),
                              glm::vec3(0, 0, 1)));
  shapes.push_back(new Point(glm::vec3(2, 2, -1), glm::vec3(0), glm::vec3(2)));
  shapes.push_back(
      new Point(glm::vec3(0, 200, -1), glm::vec3(0), glm::vec3(400)));
  glm::uvec3 **output = raytracer.render(shapes);
  while (!viewer.shouldQuit()) {
    viewer.update(output);
  }
  return 0;
}
