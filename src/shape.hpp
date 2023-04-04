#include <glm/glm.hpp>
#include <utility>

using namespace std;

class Shape
{
public:
    Shape(const glm::vec3 albedo) : albedo(albedo) {}
    virtual float intersect(const glm::vec3& o, const glm::vec3& d, const float t_min, const float t_max, glm::vec3& hit_position, glm::vec3& hit_normal) const = 0;
    glm::vec3 albedo;
};

class Sphere : public Shape
{
public:
    Sphere(const glm::vec3& center, const float radius, const glm::vec3 albedo) : Shape(albedo), center(center), radius(radius) {}
    float intersect(const glm::vec3& o, const glm::vec3& d, const float t_min, const float t_max, glm::vec3& hit_position, glm::vec3& hit_normal) const override;
private:
    glm::vec3 center;
    float radius;
};

class Plane : public Shape
{
public:
    Plane(const glm::vec3& normal, const float d, const glm::vec3 albedo) : Shape(albedo), normal(normal), d(d){}
    float intersect(const glm::vec3& o, const glm::vec3& d, const float t_min, const float t_max, glm::vec3& hit_position, glm::vec3& hit_normal) const override;
private:
    glm::vec3 normal;
    float d;
};

class Cuboid : public Shape
{
public:
    Cuboid(const glm::vec3& min, const glm::vec3& max, const glm::vec3 albedo) : Shape(albedo),min_coords(min), max_coords(max){}
    float intersect(const glm::vec3& o, const glm::vec3& d, const float t_min, const float t_max, glm::vec3& hit_position, glm::vec3& hit_normal) const override;
private:
    glm::vec3 min_coords;
    glm::vec3 max_coords;
};
