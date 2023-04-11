#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/gtc/matrix_transform.hpp>

int main() {
  RayTracer raytracer(640, 480, 90, glm::vec3(0), glm::vec3(0, 0, -1),
                      glm::vec3(0, 1, 0), true);
  Viewer viewer(640, 480, "Raytracer");
  vector<Shape *> shapes;

  // cuboid light source
  shapes.push_back(new Cuboid(glm::vec3(-1, 3.9, -7), glm::vec3(1, 4.1, -5),
                              glm::vec3(1, 1, 1), glm::vec3(3)));

  // diffuse plane y = -1
  shapes.push_back(
      new Plane(glm::vec3(0, 1, 0), -1, glm::vec3(0.6f, 0.2f, 0.2f)));
  // metal plane x = 3
  shapes.push_back(
      new Plane(glm::vec3(-1, 0, 0), -3, glm::vec3(0.3f, 0.1f, 0.1f)));
  shapes[shapes.size() - 1]->material =
      new Metallic(glm::vec3(0.2f, 0.2f, 0.8f), true);
  // metal plane x = -3
  shapes.push_back(
      new Plane(glm::vec3(1, 0, 0), -3, glm::vec3(0.3f, 0.1f, 0.1f)));
  shapes[shapes.size() - 1]->material =
      new Metallic(glm::vec3(0.2f, 0.2f, 0.8f), true);
  // metal plane z = -10
  shapes.push_back(
      new Plane(glm::vec3(0, 0, 1), -10, glm::vec3(0.3f, 0.1f, 0.1f)));
  shapes[shapes.size() - 1]->material =
      new Metallic(glm::vec3(0.2f, 0.2f, 0.8f), true);

  // rotate the cuboid
  glm::mat4 rot = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -4));
  rot = glm::rotate(rot, -PI / 3 - PI / 12, glm::vec3(0, 1, 0));
  rot = glm::translate(rot, glm::vec3(0, 0, 4));
  // transparent cuboid
  shapes.push_back(new Cuboid(glm::vec3(-1, -1, -4.5), glm::vec3(1, 1, -3.5),
                              glm::vec3(0.4f), glm::vec3(0), rot));

  shapes[shapes.size() - 1]->material = new Transparent(1.5f, true);

  // diffuse sphere
  shapes.push_back(new Sphere(glm::vec3(-2, 0, -7.5), 1.0f, glm::vec3(0.4f)));
  // glass sphere
  shapes.push_back(new Sphere(glm::vec3(-1, -0.5f, -2), 0.5f, glm::vec3(0.2f)));
  shapes[shapes.size() - 1]->material = new Transparent(1.5f, true);
  // shapes[shapes.size() - 1]->material =
  //     new Metallic(glm::vec3(0.8f, 0.2f, 0.2f), true);

  // metal cuboid enclosing everything
  // shapes.push_back(
  //    new Cuboid(glm::vec3(-5, -5, -10), glm::vec3(5, 5, 1),
  //    glm::vec3(0.4f)));
  // shapes[shapes.size() - 1]->material =
  //    new Metallic(glm::vec3(0.1f, 0.4f, 0.1f), true);

  while (!viewer.shouldQuit()) {
    glm::uvec3 **output = raytracer.render(shapes);
    viewer.update(output);
  }
  return 0;
}
