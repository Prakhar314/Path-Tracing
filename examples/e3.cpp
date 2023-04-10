#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/fwd.hpp>

int main() {
  RayTracer raytracer(640, 480, 90, glm::vec3(0), glm::vec3(0, 0, -1),
                      glm::vec3(0, 1, 0));
  Viewer viewer(640, 480, "Raytracer");
  vector<Shape *> shapes;

  Shape *sphere1 = new Sphere(glm::vec3(0, 0, -2), 1.0f, glm::vec3(1, 1, 1));
  // Material* mat = new Metallic(glm::vec3(1, 1, 1),true);
  Material *mat = new Transparent(1.5, true);
  sphere1->material = mat;
  shapes.push_back(sphere1);

  Shape *plan = new Plane(glm::vec3(0, 1, 0), -1, glm::vec3(1, 1, 1));
  // Material* matt=new Metallic(glm::vec3(1,1,1),true);
  // plan->material=matt;
  shapes.push_back(plan);

  Shape *cuboid1 = new Cuboid(glm::vec3(-0.5, -0.5, -4.5),
                              glm::vec3(4.5, 0.5, -3.5), glm::vec3(0, 0, 1));
  Material *mat1 = new Metallic(glm::vec3(0, 0, 1), true);
  // Material* mat1 = new Transparent(1.33,true);
  cuboid1->material = mat1;
  shapes.push_back(cuboid1);

  Shape *cuboid2 = new Cuboid(glm::vec3(-3, -0.0, -4), glm::vec3(-2, 0.5, -3),
                              glm::vec3(0, 0, 1));
  // cuboid2->material=mat;
  // Material* mat2 = new Metallic(glm::vec3(1, 2, 3),true);
  // cuboid2->material=mat2;
  shapes.push_back(cuboid2);

  shapes.push_back(new Point(glm::vec3(2, 2, -1), glm::vec3(0), glm::vec3(2)));
  shapes.push_back(
      new Point(glm::vec3(-10, 200, -1), glm::vec3(0), glm::vec3(800)));
  // shapes.push_back(new Point(glm::vec3(10, 20, 0), glm::vec3(0),
  // glm::vec3(500000)));
  glm::uvec3 **output = raytracer.render(shapes);
  while (!viewer.shouldQuit()) {
    viewer.update(output);
  }
  return 0;
}
