
#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main() {
  RayTracer raytracer(640, 480, 90, glm::vec3(0), glm::vec3(0, 0, -1),
                      glm::vec3(0, 1, 0));
  Viewer viewer(640, 480, "Raytracer");
  vector<Shape *> shapes;
  // transparent sphere in the middle
  shapes.push_back(new Sphere(glm::vec3(0, 0, -2), 1.0f, glm::vec3(1, 0, 0)));
  shapes[shapes.size() - 1]->material = new Transparent(1.5, true);
  // metallic sphere behind the transparent sphere
  shapes.push_back(
      new Sphere(glm::vec3(0, 0, -5), 2.0f, glm::vec3(1, 0, 0), glm::vec3(0),
                 glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 0.5f, 1.0f))));
  shapes[shapes.size() - 1]->material =
      new Metallic(glm::vec3(0.7, 0.35, 0.0), true);
  // metallic plane y =-1
  shapes.push_back(new Plane(glm::vec3(0, 1, 0), -1, glm::vec3(0.5, 0.5, 0.5)));
  shapes[shapes.size() - 1]->material = new Metallic(glm::vec3(0.1f), true);
  // point light behind the camera
  shapes.push_back(
      new Point(glm::vec3(0, 0, 1), glm::vec3(0.0f), glm::vec3(50)));
  glm::uvec3 **output = raytracer.render(shapes);
  while (!viewer.shouldQuit()) {
    viewer.update(output);
  }
  return 0;
}
