#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/fwd.hpp>
// with and without gamma correction
int main(int argc, char **argv) {
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

  Viewer viewer(width, height,
                argc == 1 ? "With gamma correction"
                          : "Without gamma correction");
  vector<Shape *> shapes;

  {
    // left wall
    Plane* plane = new Plane();
    plane->set_normal(glm::vec3(1, 0, 0));
    plane->set_d(-5);
    plane->set_albedo(glm::vec3(0.2, 0.2, 0.8));

    shapes.push_back(plane);
  }
  {
    // right wall
    Plane* plane = new Plane();
    plane->set_normal(glm::vec3(-1, 0, 0));
    plane->set_d(-5);
    plane->set_albedo(glm::vec3(0.8, 0.2, 0.2));

    shapes.push_back(plane);
  }
  {
    // floor
    Plane* plane = new Plane();
    plane->set_normal(glm::vec3(0, 1, 0));
    plane->set_d(-5);
    plane->set_albedo(glm::vec3(0.8, 0.8, 0.8));

    shapes.push_back(plane);
  }
  {
    // ceiling
    Plane* plane = new Plane();
    plane->set_normal(glm::vec3(0, -1, 0));
    plane->set_d(-5);
    plane->set_albedo(glm::vec3(0.4, 0.4, 0.4));

    shapes.push_back(plane);
  }
  {
    // back wall
    Plane* plane = new Plane();
    plane->set_normal(glm::vec3(0, 0, 1));
    plane->set_d(-13);
    plane->set_albedo(glm::vec3(0.2, 0.8, 0.2));

    shapes.push_back(plane);
  }
  {
    // metallic sphere 1
    Sphere* sphere = new Sphere();
    sphere->set_center(glm::vec3(2, -3, -6));
    sphere->set_radius(2.0f);
    sphere->set_albedo(glm::vec3(1.0, 0.0, 0.0));

    Material* material = new Metallic(glm::vec3(0.5, 0.5, 0.5));
    sphere->set_material(material);

    shapes.push_back(sphere);
  }
  {
    // metallic sphere 2
    Sphere* sphere = new Sphere();
    sphere->set_center(glm::vec3(-3, -3, -9));
    sphere->set_radius(2.0f);
    sphere->set_albedo(glm::vec3(1.0, 0.0, 0.0));

    Material* material = new Metallic(glm::vec3(0.5, 0.5, 0.5));
    sphere->set_material(material);

    shapes.push_back(sphere);
  }
  {
    // point light 1
    Point* point = new Point();
    point->set_position(glm::vec3(-2, 4.9, -10));
    point->set_intensity(glm::vec3(4.5));
    point->set_albedo(glm::vec3(0));

    shapes.push_back(point);
  }
  {
    // point light 2
    Point* point = new Point();
    point->set_position(glm::vec3(0, 4.9, -10));
    point->set_intensity(glm::vec3(4.5));
    point->set_albedo(glm::vec3(0));

    shapes.push_back(point);
  }
  {
    // point light 3
    Point* point = new Point();
    point->set_position(glm::vec3(2, 4.9, -10));
    point->set_intensity(glm::vec3(4.5));
    point->set_albedo(glm::vec3(0));

    shapes.push_back(point);
  }
  glm::uvec3 **output;
  output = raytracer.render(shapes, 16, argc == 1);
  while (!viewer.shouldQuit()) {
    viewer.update(output);
  }
  return 0;
}
