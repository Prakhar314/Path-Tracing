#include <glm/glm.hpp>
#include <utility>

using namespace std;

class Shape
{
public:
    Shape(const glm::vec3 albedo, const glm::vec3 intensity = glm::vec3(0)) : 
    albedo(albedo), intensity(intensity), is_light(glm::length(intensity) > 0.001f){}
    virtual float intersect(const glm::vec3& o, const glm::vec3& d, const float t_min, const float t_max, glm::vec3& hit_normal) const = 0;
    virtual glm::vec3 get_position() const{
      throw "Not implemented";
    }
    const glm::vec3 albedo;
    const glm::vec3 intensity;
    const bool is_light;
};

class Sphere : public Shape
{
public:
    Sphere(const glm::vec3& center, const float radius, const glm::vec3 albedo, const glm::vec3 intensity = glm::vec3(0)) : Shape(albedo, intensity), center(center), radius(radius) {}
    glm::vec3 get_position() const override;
    float intersect(const glm::vec3& o, const glm::vec3& d, const float t_min, const float t_max, glm::vec3& hit_normal) const override;
private:
    const glm::vec3 center;
    const float radius;
};

class Plane : public Shape
{
public:
    Plane(const glm::vec3& normal, const float d, const glm::vec3 albedo, const glm::vec3 intensity = glm::vec3(0)) : Shape(albedo, intensity), normal(normal), d(d){}
    float intersect(const glm::vec3& o, const glm::vec3& d, const float t_min, const float t_max, glm::vec3& hit_normal) const override;
private:
    const glm::vec3 normal;
    const float d;
};

class Cuboid : public Shape
{
public:
    Cuboid(const glm::vec3& min, const glm::vec3& max, const glm::vec3 albedo, const glm::vec3 intensity = glm::vec3(0)) : Shape(albedo, intensity),min_coords(min), max_coords(max){}
    float intersect(const glm::vec3& o, const glm::vec3& d, const float t_min, const float t_max, glm::vec3& hit_normal) const override;
private:
    const glm::vec3 min_coords;
    const glm::vec3 max_coords;
};

class Point : public Shape
{
public:
    Point(const glm::vec3& position, const glm::vec3 albedo = glm::vec3(0), const glm::vec3 intensity = glm::vec3(0)) : Shape(albedo, intensity), position(position){}
    float intersect(const glm::vec3& o, const glm::vec3& d, const float t_min, const float t_max, glm::vec3& hit_normal) const override;
    glm::vec3 get_position() const override;
private:
    const glm::vec3 position;
};
