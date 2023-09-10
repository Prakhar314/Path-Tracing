#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/fwd.hpp>
// metal vs glass
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

  Viewer viewer(width, height, "Green Metal and Glass");
  vector<Shape *> shapes;
  // make a box using 5 planes

  {
    // left wall
    Plane *plane = new Plane();
    plane->set_normal(glm::vec3(1, 0, 0));
    plane->set_d(-5);
    plane->set_albedo(glm::vec3(0.2, 0.2, 0.8));

    shapes.push_back(plane);
  }
  {
    // right wall
    Plane *plane = new Plane();
    plane->set_normal(glm::vec3(-1, 0, 0));
    plane->set_d(-5);
    plane->set_albedo(glm::vec3(0.8, 0.2, 0.2));

    shapes.push_back(plane);
  }
  {
    // floor
    Plane *plane = new Plane();
    plane->set_normal(glm::vec3(0, 1, 0));
    plane->set_d(-5);
    plane->set_albedo(glm::vec3(0.8, 0.8, 0.8));

    shapes.push_back(plane);
  }
  {
    // ceiling
    Plane *plane = new Plane();
    plane->set_normal(glm::vec3(0, -1, 0));
    plane->set_d(-5);
    plane->set_albedo(glm::vec3(0.4, 0.4, 0.4));

    shapes.push_back(plane);
  }
  {
    // back wall
    Plane *plane = new Plane();
    plane->set_normal(glm::vec3(0, 0, 1));
    plane->set_d(-13);
    plane->set_albedo(glm::vec3(0.2, 0.8, 0.2));

    shapes.push_back(plane);
  }
  {
    // glass sphere
    Sphere *sphere = new Sphere();
    sphere->set_center(glm::vec3(-2.5, -3, -7));
    sphere->set_radius(2.0f);
    sphere->set_albedo(glm::vec3(1, 0, 0));

    Material *material = new Transparent(1.5f);
    sphere->set_material(material);

    shapes.push_back(sphere);
  }
  {
    // metal sphere
    Sphere *sphere = new Sphere();
    sphere->set_center(glm::vec3(2, -3, -9));
    sphere->set_radius(2.0f);
    sphere->set_albedo(glm::vec3(1, 0, 0));

    Material *material = new Metallic(glm::vec3(0.2, 0.8, 0.2));
    sphere->set_material(material);

    shapes.push_back(sphere);
  }

  {
    // light 1
    Point *point = new Point();
    point->set_position(glm::vec3(0, 4.9, -10));
    point->set_intensity(glm::vec3(7.5));
    point->set_albedo(glm::vec3(0));

    shapes.push_back(point);
  }
  {
    // light 2
    Point *point = new Point();
    point->set_position(glm::vec3(0, 4.9, -7.5));
    point->set_intensity(glm::vec3(7.5));
    point->set_albedo(glm::vec3(0));

    shapes.push_back(point);
  }
  {
    // light 3
    Point *point = new Point();
    point->set_position(glm::vec3(0, 4.9, -5));
    point->set_intensity(glm::vec3(7.5));
    point->set_albedo(glm::vec3(0));

    shapes.push_back(point);
  }

  glm::uvec3 **output;
  raytracer.init(shapes);
  output = raytracer.render();
  while (!viewer.shouldQuit()) {
    viewer.update(output);
  }
  return 0;
}
