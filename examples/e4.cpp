#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/gtc/matrix_transform.hpp>

// affince transformations

int main() {
  RayTracer raytracer(640, 480, 90, glm::vec3(0), glm::vec3(0, 0, -1),
                      glm::vec3(0, 1, 0));
  Viewer viewer(640, 480, "Transformations");
  vector<Shape *> shapes;

  glm::mat4 stretch = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -2));
  stretch = glm::scale(stretch, glm::vec3(1, 0.5, 0.5));
  stretch = glm::translate(stretch, glm::vec3(0, 0, 2));

  shapes.push_back(new Sphere(glm::vec3(0, 0, -2), 1.0f, glm::vec3(0.8f),
                              glm::vec3(0), stretch));

  stretch = glm::translate(glm::mat4(1.0f), glm::vec3(-1, 1.5, -3));
  stretch = glm::scale(stretch, glm::vec3(0.5, 1, 1));
  stretch = glm::translate(stretch, glm::vec3(1, -1.5, 3));

  shapes.push_back(new Sphere(glm::vec3(-1, 1.5, -3), 1.0f, glm::vec3(0.8f),
                              glm::vec3(0), stretch));

  shapes.push_back(new Plane(glm::vec3(0, 1, 0), -1, glm::vec3(0.8f)));

  glm::mat4 rot = glm::translate(glm::mat4(1.0f), glm::vec3(2.5, 1.5, -3.5));
  rot = glm::rotate(rot, -PI / 3, glm::vec3(0, 1, 0));
  rot = glm::rotate(rot, PI / 4, glm::vec3(1, 0, 0));
  rot = glm::translate(rot, glm::vec3(-2.5, -1.5, 3.5));

  shapes.push_back(new Cuboid(glm::vec3(2, 1, -4), glm::vec3(3, 2, -3),
                              glm::vec3(0.8f), glm::vec3(0), rot));

  rot = glm::translate(glm::mat4(1.0f), glm::vec3(-2.5, 0, -3.5));
  rot = glm::rotate(rot, PI / 4, glm::vec3(0, 1, 0));
  rot = glm::translate(rot, glm::vec3(2.5, 0, 3.5));

  shapes.push_back(new Cuboid(glm::vec3(-3, -1, -4), glm::vec3(-2, -0.5, -3),
                              glm::vec3(0.8f), glm::vec3(0), rot));
  // near by point light
  shapes.push_back(new Point(glm::vec3(5, 5, -1), glm::vec3(1), glm::vec3(6)));
  // far waya point lgiht
  shapes.push_back(
      new Point(glm::vec3(0, 100, -5), glm::vec3(1), glm::vec3(600)));
  glm::uvec3 **output = raytracer.render(shapes);
  while (!viewer.shouldQuit()) {
    viewer.update(output);
  }
  return 0;
}
