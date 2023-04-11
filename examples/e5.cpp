#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/fwd.hpp>
// with and without gamma correction
int main(int argc, char **argv) {
  RayTracer raytracer(640, 480, 90, glm::vec3(0), glm::vec3(0, 0, -1),
                      glm::vec3(0, 1, 0));
  Viewer viewer(640, 480,
                argc == 1 ? "With gamma correction"
                          : "Without gamma correction");
  vector<Shape *> shapes;
  // make a box using 5 planes
  shapes.push_back(new Plane(glm::vec3(1, 0, 0), -5, glm::vec3(0.2, 0.2, 0.8)));
  shapes.push_back(
      new Plane(glm::vec3(-1, 0, 0), -5, glm::vec3(0.8, 0.2, 0.2)));
  shapes.push_back(new Plane(glm::vec3(0, 1, 0), -5, glm::vec3(0.8, 0.8, 0.8)));
  shapes.push_back(
      new Plane(glm::vec3(0, -1, 0), -5, glm::vec3(0.4, 0.4, 0.4)));
  shapes.push_back(
      new Plane(glm::vec3(0, 0, 1), -13, glm::vec3(0.2, 0.8, 0.2)));

  shapes.push_back(new Sphere(glm::vec3(2, -3, -6), 2.0f, glm::vec3(1, 0, 0)));
  shapes[shapes.size() - 1]->material =
      new Metallic(glm::vec3(0.5, 0.5, 0.5), true);
  shapes.push_back(new Sphere(glm::vec3(-3, -3, -9), 2.0f, glm::vec3(1, 0, 0)));
  shapes[shapes.size() - 1]->material =
      new Metallic(glm::vec3(0.5, 0.5, 0.5), true);

  shapes.push_back(
      new Point(glm::vec3(-2, 4.9, -10), glm::vec3(0), glm::vec3(4.5)));
  shapes.push_back(
      new Point(glm::vec3(0, 4.9, -10), glm::vec3(0), glm::vec3(4.5)));
  shapes.push_back(
      new Point(glm::vec3(2, 4.9, -10), glm::vec3(0), glm::vec3(4.5)));
  glm::uvec3 **output;
  output = raytracer.render(shapes, 16, argc == 1);
  while (!viewer.shouldQuit()) {
    viewer.update(output);
  }
  return 0;
}
