#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/gtc/matrix_transform.hpp>
// diffuse Interreflections
int main() {
  RayTracer raytracer(640, 480, 90, glm::vec3(0), glm::vec3(0, 0, -1),
                      glm::vec3(0, 1, 0), true);
  Viewer viewer(640, 480, "Diffuse Interreflections");
  vector<Shape *> shapes;
  // make a box using 5 planes
  shapes.push_back(new Plane(glm::vec3(1, 0, 0), -5, glm::vec3(0.2, 0.2, 0.8)));
  shapes.push_back(
      new Plane(glm::vec3(-1, 0, 0), -5, glm::vec3(0.8, 0.2, 0.2)));
  shapes.push_back(new Plane(glm::vec3(0, 1, 0), -5, glm::vec3(0.4, 0.4, 0.4)));
  shapes.push_back(
      new Plane(glm::vec3(0, -1, 0), -5, glm::vec3(0.4, 0.4, 0.4)));
  shapes.push_back(
      new Plane(glm::vec3(0, 0, 1), -11, glm::vec3(0.4, 0.4, 0.4)));

  // shapes.push_back(new Sphere(glm::vec3(-0, -2, -6), 2.0f, glm::vec3(1, 0,
  // 0))); shapes[shapes.size() - 1]->material = new Transparent(1.5, true);
  // shapes.push_back(new Sphere(glm::vec3(-3, -3, -9), 2.0f, glm::vec3(1, 0,
  // 0))); shapes[shapes.size() - 1]->material =
  //     new Metallic(glm::vec3(0.5, 0.5, 0.5), true);
  // rotate the cuboid by 45 degrees along z axis
  glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -10));
  model = glm::rotate(model, 3.14159265f / 4, glm::vec3(0, 1, 0));
  // model = glm::rotate(model, 3.14159265f / 4, glm::vec3(1, 0, 0));
  model = glm::translate(model, glm::vec3(0, 0, 10));
  shapes.push_back(new Cuboid(glm::vec3(-2, -5, -12), glm::vec3(2, 5, -8),
                              glm::vec3(0.4, 0.4, 0.4), glm::vec3(0), model));

  // area light
  shapes.push_back(new Cuboid(glm::vec3(-2.5, 4.9, -9.5),
                              glm::vec3(2.5, 5.1, -6.0), glm::vec3(0),
                              glm::vec3(1.5)));
  while (!viewer.shouldQuit()) {
    glm::uvec3 **output = raytracer.render(shapes);
    viewer.update(output);
  }
  return 0;
}
