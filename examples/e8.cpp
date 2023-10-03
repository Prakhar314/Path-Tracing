#include "../src/ray.hpp"
#include "../src/viewer.hpp"
#include <glm/gtc/matrix_transform.hpp>

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

  Viewer viewer(width, height, "Raytracer");
  vector<Shape *> shapes;

  {
    // cuboid light source
    Cuboid *cuboid = new Cuboid();
    cuboid->set_bounds(glm::vec3(-1, 3.9, -7), glm::vec3(1, 4.1, -5));
    cuboid->set_albedo(glm::vec3(0));
    cuboid->set_intensity(glm::vec3(3));

    shapes.push_back(cuboid);
  }
  {
    // diffuse plane y = -1
    Plane *plane = new Plane();
    plane->set_normal(glm::vec3(0, 1, 0));
    plane->set_d(-1);
    plane->set_albedo(glm::vec3(0.6f, 0.2f, 0.2f));

    shapes.push_back(plane);
  }
  {
    // metal plane x = 3
    Plane *plane = new Plane();
    plane->set_normal(glm::vec3(-1, 0, 0));
    plane->set_d(-3);
    plane->set_albedo(glm::vec3(0.3f, 0.1f, 0.1f));

    Material *material = new Metallic(glm::vec3(0.2f, 0.2f, 0.8f));
    plane->set_material(material);

    shapes.push_back(plane);
  }
  {
    // metal plane x = -3
    Plane *plane = new Plane();
    plane->set_normal(glm::vec3(1, 0, 0));
    plane->set_d(-3);
    plane->set_albedo(glm::vec3(0.3f, 0.1f, 0.1f));

    Material *material = new Metallic(glm::vec3(0.2f, 0.2f, 0.8f));
    plane->set_material(material);

    shapes.push_back(plane);
  }
  {
    // metal plane z = -10
    Plane *plane = new Plane();
    plane->set_normal(glm::vec3(0, 0, 1));
    plane->set_d(-10);
    plane->set_albedo(glm::vec3(0.3f, 0.1f, 0.1f));

    Material *material = new Metallic(glm::vec3(0.2f, 0.2f, 0.8f));
    plane->set_material(material);

    shapes.push_back(plane);
  }
  {
    // transparent cuboid
    Cuboid *cuboid = new Cuboid();
    cuboid->set_bounds(glm::vec3(-1, -1, -4.5), glm::vec3(1, 1, -3.5));
    cuboid->set_albedo(glm::vec3(0.4f));
    cuboid->set_intensity(glm::vec3(0));

    // rotate the cuboid
    glm::mat4 rot = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -4));
    rot = glm::rotate(rot, -PI / 3 - PI / 12, glm::vec3(0, 1, 0));
    rot = glm::translate(rot, glm::vec3(0, 0, 4));
    cuboid->set_transform(rot);

    Material *material = new Transparent(1.5f);
    cuboid->set_material(material);

    shapes.push_back(cuboid);
  }
  {
    // diffuse Sphere
    Sphere *sphere = new Sphere();
    sphere->set_center(glm::vec3(-2, 0, -7.5));
    sphere->set_radius(1.0f);
    sphere->set_albedo(glm::vec3(0.4f));

    shapes.push_back(sphere);
  }
  {
    Sphere *sphere = new Sphere();
    sphere->set_center(glm::vec3(-1, -0.5f, -2));
    sphere->set_radius(0.5f);
    sphere->set_albedo(glm::vec3(0.2f));

    Material *material = new Transparent(1.5f);
    sphere->set_material(material);

    shapes.push_back(sphere);
  }
    
  raytracer.set_num_threads(8);

  raytracer.init(shapes);
  uint32_t avg = 0;
  uint32_t count = 0;
  while (!viewer.shouldQuit()) {
    uint32_t start = SDL_GetTicks();
    glm::uvec3 **output = raytracer.render();
    uint32_t end = SDL_GetTicks();
    avg += end - start;
    count++;
    cout << "Average time: " << avg / count << endl;
    viewer.update(output);
  }
  return 0;
}
