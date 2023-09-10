#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/gtc/matrix_transform.hpp>
// diffuse Interreflections
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
  raytracer.set_path_tracing(true);

  Viewer viewer(width, height, "Diffuse Interreflections");
  vector<Shape *> shapes;

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
    plane->set_albedo(glm::vec3(0.4, 0.4, 0.4));

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
    plane->set_d(-11);
    plane->set_albedo(glm::vec3(0.4, 0.4, 0.4));

    shapes.push_back(plane);
  }
  {
    // big rotated cuboid
    Cuboid *cuboid = new Cuboid();
    cuboid->set_bounds(glm::vec3(-2, -5, -12), glm::vec3(2, 5, -8));
    cuboid->set_albedo(glm::vec3(0.4, 0.4, 0.4));
    // rotate the cuboid by 45 degrees along z axis
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -10));
    model = glm::rotate(model, 3.14159265f / 4, glm::vec3(0, 1, 0));
    // model = glm::rotate(model, 3.14159265f / 4, glm::vec3(1, 0, 0));
    model = glm::translate(model, glm::vec3(0, 0, 10));
    cuboid->set_transform(model);

    shapes.push_back(cuboid);
  }
  {
    // area light
    Cuboid *cuboid = new Cuboid();
    cuboid->set_bounds(glm::vec3(-2.5, 4.9, -9.5), glm::vec3(2.5, 5.1, -6.0));
    cuboid->set_albedo(glm::vec3(0));
    cuboid->set_intensity(glm::vec3(1.5));
    shapes.push_back(cuboid);
  }

  raytracer.init(shapes);
  while (!viewer.shouldQuit()) {
    glm::uvec3 **output = raytracer.render();
    viewer.update(output);
  }
  return 0;
}
