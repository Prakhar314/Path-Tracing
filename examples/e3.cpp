#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/fwd.hpp>

int main() {
  RayTracer raytracer(640, 480, 90, glm::vec3(0), glm::vec3(0, 0, -1),
                      glm::vec3(0, 1, 0));
  Viewer viewer(640, 480, "Raytracer");
  vector<Shape *> shapes;
  shapes.push_back(new Sphere(glm::vec3(0, 0, -2), 1.0f, glm::vec3(0.8f)));
  shapes.push_back(new Plane(glm::vec3(0, 1, 0), -1, glm::vec3(0.8f)));
  shapes.push_back(
      new Cuboid(glm::vec3(2, 1, -4), glm::vec3(3, 2, -3), glm::vec3(0.8f)));
  shapes.push_back(new Cuboid(glm::vec3(-3, -1, -4), glm::vec3(-2, -0.5, -3),
                              glm::vec3(0.8f)));
  // near by point light
  shapes.push_back(new Point(glm::vec3(5, 5, -1), glm::vec3(1), glm::vec3(6)));
  // far away point lgiht
  shapes.push_back(
      new Point(glm::vec3(0, 100, -5), glm::vec3(1), glm::vec3(600)));
  glm::uvec3 **output = raytracer.render(shapes);
  while (!viewer.shouldQuit()) {
    viewer.update(output);
  }
  return 0;
}
