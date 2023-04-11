#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>
#include <math.h>

using namespace std;
class Material {
public:
  Material(const bool met = false, const bool trans = false)
      : is_metallic(met), is_transparent(trans) {}
  virtual ~Material() {}
  virtual glm::vec3 reflect(glm::vec3 n, glm::vec3 i) const {
    throw "Not implemented";
  }

  virtual glm::vec3 transmit(glm::vec3 n, glm::vec3 i) const {
    throw "Not implemented";
  }

  virtual glm::vec3 reflectance(glm::vec3 n, glm::vec3 i) const {
    throw "Not implemented";
  }

  virtual glm::vec3 transmittance(glm::vec3 n, glm::vec3 i) const {
    throw "Not implemented";
  }
  virtual float geteta() const { throw "Not implemented"; }
  const bool is_metallic;
  const bool is_transparent;
};

class Metallic : public Material {
public:
  Metallic(glm::vec3 reflectance, bool met) : Material(met, false) {
    float maxComponent =
        max(max(reflectance.x, max(reflectance.y, 1.0f)), reflectance.z);
    f0_reflectance = reflectance / maxComponent;
  }

  // get_reflected_ray
  glm::vec3 reflect(const glm::vec3 normal,
                    const glm::vec3 incidentdirection) const override {

    // calculate reflected direction
    glm::vec3 reflected_ray =
        glm::normalize(incidentdirection -
                       2.0f * glm::dot(incidentdirection, normal) * normal);

    return reflected_ray;
  }
  glm::vec3 reflectance(const glm::vec3 normal,
                        const glm::vec3 incidentdirection) const override {

    // calculate incident angle
    float cosTheta =
        std::abs(glm::dot(normal, incidentdirection) /
                 (glm::length(normal) * glm::length(incidentdirection)));
    float theta = std::acos(cosTheta); // in radians

    // calculate reflectance
    glm::vec3 reflectance =
        this->f0_reflectance + (1.0f - this->f0_reflectance) * pow((1 - cosTheta),5.0f);
    return reflectance;
  }

private:
  glm::vec3 f0_reflectance;
};

class Transparent : public Material {
public:
  Transparent(float ri, bool met)
      : Material(false, met), refractive_index(ri) {}

  // get_reflected_ray
  glm::vec3 reflect(const glm::vec3 normal,
                    const glm::vec3 incidentdirection) const override {

    // calculate reflected direction
    glm::vec3 reflected_ray =
        glm::normalize(incidentdirection -
                       2.0f * glm::dot(incidentdirection, normal) * normal);

    return reflected_ray;
  }
  float geteta() const override { return this->refractive_index; }
  glm::vec3 transmit(const glm::vec3 normal_t,
                     const glm::vec3 incident) const override {

    float eta = this->geteta();
    float f = glm::dot(incident, normal_t);
    // determine whether ray entering or exiting
    float eta_ratio = f < 0 ? (1.0 / eta) : eta;
    // face normal, not outwards normal
    glm::vec3 normal = f < 0 ? normal_t : -normal_t;
    float cos_theta = fmin(glm::dot(-incident, normal), 1.0);
    float sin_theta = sqrt(1.0 - cos_theta * cos_theta);
    if (eta_ratio * sin_theta > 1.0) {
      // Total internal reflection
      return this->reflect(normal, incident);
    } else {
      float cos_phi = sqrt(1.0 - eta_ratio * eta_ratio * sin_theta * sin_theta);
      glm::vec3 refracted_dir =
          eta_ratio * incident + (eta_ratio * cos_theta - cos_phi) * normal;
      return refracted_dir;
    }
  }

  glm::vec3 reflectance(const glm::vec3 normal,
                        const glm::vec3 incidentdirection) const override {

    glm::vec3 f0_reflectance(
        ((this->refractive_index - 1) * (this->refractive_index - 1)) /
        ((this->refractive_index + 1) * (this->refractive_index + 1)));
    // std::cout<<f0_reflectance.x<<"\n";
    // calculate incident angle
    float cosTheta =
        std::abs(glm::dot(normal, incidentdirection) /
                 (glm::length(normal) * glm::length(incidentdirection)));

    // calculate reflectance
    glm::vec3 reflectance =
        f0_reflectance + (1.0f - f0_reflectance) * pow((1 - cosTheta),5.0f);
    return reflectance;
  }
  glm::vec3 transmittance(const glm::vec3 normal,
                          const glm::vec3 incidentdirection) const override {

    return 1.0f - this->reflectance(normal, incidentdirection);
  }

private:
  float refractive_index;
};
