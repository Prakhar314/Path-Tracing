#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/fwd.hpp>

int main() {
  int width = 640;
  int height = 480;
  int vfov = 90;
  glm::vec3 camera_direction(0, 0, -1);
  glm::vec3 camera_up(0, 1, 0);
  glm::vec3 camera_position(0, 0, 0);

  RayTracer raytracer;
  raytracer.set_resolution(width, height);
  raytracer.set_fov(vfov);
  raytracer.set_camera(camera_position, camera_direction, camera_up);
  raytracer.set_path_tracing(false);
  raytracer.init();

  Viewer viewer(width, height, "Raytracer");
  vector<Shape *> shapes;
  {
    // small sphere
    Sphere *sphere = new Sphere();
    sphere->set_center(glm::vec3(0, 0, -2));
    sphere->set_radius(1.0f);
    sphere->set_albedo(glm::vec3(1, 0, 0));
    shapes.push_back(sphere);
  }
  {
    // big sphere
    Sphere *sphere = new Sphere();
    sphere->set_center(glm::vec3(0, -101, -2));
    sphere->set_radius(100.0f);
    sphere->set_albedo(glm::vec3(0, 1, 0));
    shapes.push_back(sphere);
  }
  glm::uvec3 **output = raytracer.render(shapes);
  while (!viewer.shouldQuit()) {
    viewer.update(output);
  }
  // Free memory
  for (size_t i = 0; i < shapes.size(); i++) {
    delete shapes[i];
  }
  return 0;
}
