#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/gtc/matrix_transform.hpp>

// affince transformations

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
  Viewer viewer(width, height, "Raytracer");
  vector<Shape *> shapes;

  {
    // stretched sphere 1
    Sphere *sphere = new Sphere();
    sphere->set_center(glm::vec3(0, 0, -2));
    sphere->set_radius(1.0f);
    sphere->set_albedo(glm::vec3(0.8f));

    glm::mat4 stretch = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -2));
    stretch = glm::scale(stretch, glm::vec3(1, 0.5, 0.5));
    stretch = glm::translate(stretch, glm::vec3(0, 0, 2));

    sphere->set_transform(stretch);

    shapes.push_back(sphere);
  }

  {
    // stretched sphere 2
    Sphere *sphere = new Sphere();
    sphere->set_center(glm::vec3(0, 2, -3));
    sphere->set_radius(1.0f);
    sphere->set_albedo(glm::vec3(0.8f));

    glm::mat4 stretch = glm::translate(glm::mat4(1.0f), glm::vec3(0, 2, -3));
    stretch = glm::scale(stretch, glm::vec3(0.5, 1, 1));
    stretch = glm::translate(stretch, glm::vec3(0, -2, 3));

    sphere->set_transform(stretch);

    shapes.push_back(sphere);
  }
  {
    // platform
    Plane *plane = new Plane();
    plane->set_normal(glm::vec3(0, 1, 0));
    plane->set_d(-1);
    plane->set_albedo(glm::vec3(0.8f));

    shapes.push_back(plane);
  }
  {
    // cuboid 1
    Cuboid *cuboid = new Cuboid();
    cuboid->set_bounds(glm::vec3(2, 1, -4), glm::vec3(3, 2, -3));
    cuboid->set_albedo(glm::vec3(0.8f));

    glm::mat4 rot = glm::translate(glm::mat4(1.0f), glm::vec3(2.5, 1.5, -3.5));
    rot = glm::rotate(rot, -PI / 3, glm::vec3(0, 1, 0));
    rot = glm::rotate(rot, PI / 4, glm::vec3(1, 0, 0));
    rot = glm::translate(rot, glm::vec3(-2.5, -1.5, 3.5));

    cuboid->set_transform(rot);

    shapes.push_back(cuboid);
  }
  {
    // cuboid 2
    Cuboid *cuboid = new Cuboid();
    cuboid->set_bounds(glm::vec3(-3, -1, -4), glm::vec3(-2, -0.5, -3));
    cuboid->set_albedo(glm::vec3(0.8f));

    glm::mat4 rot = glm::translate(glm::mat4(1.0f), glm::vec3(-2.5, 0, -3.5));
    rot = glm::rotate(rot, PI / 4, glm::vec3(0, 1, 0));
    rot = glm::translate(rot, glm::vec3(2.5, 0, 3.5));

    cuboid->set_transform(rot);

    shapes.push_back(cuboid);
  }
  {
    // near by point light
    Point *point = new Point();
    point->set_position(glm::vec3(5, 5, -1));
    point->set_albedo(glm::vec3(1));
    point->set_intensity(glm::vec3(6));

    shapes.push_back(point);
  }
  {
    // far away point lgiht
    Point *point = new Point();
    point->set_position(glm::vec3(0, 100, -5));
    point->set_albedo(glm::vec3(1));
    point->set_intensity(glm::vec3(600));

    shapes.push_back(point);
  }

  raytracer.init(shapes);
  glm::uvec3 **output = raytracer.render();
  while (!viewer.shouldQuit()) {
    viewer.update(output);
  }

  // clean up
  for (size_t i = 0; i < shapes.size(); i++) {
    delete shapes[i];
  }
  return 0;
}
