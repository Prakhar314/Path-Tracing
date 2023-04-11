#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/fwd.hpp>
// metal vs glass
int main(int argc, char **argv) {
  RayTracer raytracer(640, 480, 90, glm::vec3(0), glm::vec3(0, 0, -1),
                      glm::vec3(0, 1, 0));
  Viewer viewer(640, 480, "Green Metal and Glass");
  vector<Shape *> shapes;
  // make a box using 5 planes
  shapes.push_back(new Plane(glm::vec3(1, 0, 0), -5, glm::vec3(0.1, 0.1, 0.4)));
  shapes.push_back(
      new Plane(glm::vec3(-1, 0, 0), -5, glm::vec3(0.4, 0.1, 0.1)));
  shapes.push_back(new Plane(glm::vec3(0, 1, 0), -5, glm::vec3(0.4, 0.4, 0.4)));
  shapes.push_back(
      new Plane(glm::vec3(0, -1, 0), -5, glm::vec3(0.2, 0.2, 0.2)));
  shapes.push_back(
      new Plane(glm::vec3(0, 0, 1), -13, glm::vec3(0.2, 0.2, 0.2)));

  shapes.push_back(
      new Sphere(glm::vec3(-2.5, -3, -7), 2.0f, glm::vec3(1, 0, 0)));
  shapes[shapes.size() - 1]->material = new Transparent(1.5f, true);
  shapes.push_back(new Sphere(glm::vec3(2, -3, -9), 2.0f, glm::vec3(1, 0, 0)));
  shapes[shapes.size() - 1]->material =
      new Metallic(glm::vec3(0.2, 0.8, 0.2), true);

  shapes.push_back(
      new Point(glm::vec3(0, 4.9, -10), glm::vec3(0), glm::vec3(5)));
  shapes.push_back(
      new Point(glm::vec3(0, 4.9, -7.5), glm::vec3(0), glm::vec3(5)));
  shapes.push_back(
      new Point(glm::vec3(0, 4.9, -5), glm::vec3(0), glm::vec3(5)));
  glm::uvec3 **output;
  output = raytracer.render(shapes);
  while (!viewer.shouldQuit()) {
    viewer.update(output);
  }
  return 0;
}
