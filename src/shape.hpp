#include <glm/glm.hpp>
#include <utility>

using namespace std;

class Shape
{
public:
    virtual pair<glm::vec3, glm::vec3> intersect(const glm::vec3& o, const glm::vec3& d, const float t_min, const float t_max) const = 0;
private:
    glm::vec3 albedo;
};

class Sphere : public Shape
{
public:
    Sphere(const glm::vec3& center, const float radius) : center(center), radius(radius) {}
    pair<glm::vec3, glm::vec3> intersect(const glm::vec3& o, const glm::vec3& d, const float t_min, const float t_max) const override;
private:
    glm::vec3 center;
    float radius;
};

class Plane : public Shape
{
public:
    Plane(const glm::vec3& normal, const float d) : normal(normal), d(d) {}
    pair<glm::vec3, glm::vec3> intersect(const glm::vec3& o, const glm::vec3& d, const float t_min, const float t_max) const override;
private:
    glm::vec3 normal;
    float d;
};

class Cuboid : public Shape
{
public:
    Cuboid(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}
    pair<glm::vec3, glm::vec3> intersect(const glm::vec3& o, const glm::vec3& d, const float t_min, const float t_max) const override;
private:
    glm::vec3 min;
    glm::vec3 max;
};
