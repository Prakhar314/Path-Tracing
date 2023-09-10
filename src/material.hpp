#include <algorithm>
#include <glm/glm.hpp>
#include <iostream>
#include <math.h>

#define UNUSED __attribute__((unused))

using namespace std;
class Material {
public:
  virtual ~Material() {}

  virtual glm::vec3 reflect(glm::vec3 n, glm::vec3 i) const = 0;
  virtual glm::vec3 transmit(glm::vec3 n, glm::vec3 i) const = 0;
  virtual glm::vec3 reflectance(glm::vec3 n, glm::vec3 i) const = 0;
  virtual glm::vec3 transmittance(glm::vec3 n, glm::vec3 i) const = 0;
  virtual float geteta() const = 0;
  virtual bool isMetallic() const { return false; }
  virtual bool isTransparent() const { return false; }
};

class Diffuse : public Material {
  glm::vec3 reflect(UNUSED glm::vec3 n,UNUSED glm::vec3 i) const override {
    throw runtime_error("Diffuse material does not reflect light");
  }
  glm::vec3 transmit(UNUSED glm::vec3 n,UNUSED glm::vec3 i) const override {
    throw runtime_error("Diffuse material does not transmit light");
  }
  glm::vec3 reflectance(UNUSED glm::vec3 n,UNUSED  glm::vec3 i) const override {
    throw runtime_error("Diffuse material does not reflect light");
  }
  glm::vec3 transmittance(UNUSED glm::vec3 n,UNUSED  glm::vec3 i) const override {
    throw runtime_error("Diffuse material does not transmit light");
  }
  float geteta() const override {
    throw runtime_error("Diffuse material does not transmit light");
  }
};

class Metallic : public Material {
public:
  explicit Metallic(glm::vec3 reflectance) {
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
    float cosTheta = std::abs(glm::dot(normal, incidentdirection));

    // calculate reflectance
    glm::vec3 reflectance =
        this->f0_reflectance +
        (1.0f - this->f0_reflectance) * pow((1 - cosTheta), 5.0f);
    return reflectance;
  }

  glm::vec3 transmit(UNUSED const glm::vec3 normal,
                     UNUSED const glm::vec3 incidentdirection) const override {

    throw runtime_error("Metallic material does not transmit light");
  }

  glm::vec3 transmittance(UNUSED const glm::vec3 normal,
                          UNUSED const glm::vec3 incidentdirection) const override {

    throw runtime_error("Metallic material does not transmit light");
  }

  float geteta() const override {
    throw runtime_error("Metallic material does not transmit light");
  }

  bool isMetallic() const override { return true; }

private:
  glm::vec3 f0_reflectance;
};

class Transparent : public Material {
public:
  explicit Transparent(float ri) : refractive_index(ri) {}

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
    float cosTheta = glm::dot(normal, incidentdirection);
    // calculate transmitted angle
    float eta = this->geteta();
    if (cosTheta < 0) {
      eta = 1.0 / eta;
      cosTheta = -cosTheta;
    }
    float sinThetaT = eta * sqrt(1.0 - cosTheta * cosTheta);
    if (sinThetaT >= 1.0) {
      // Total internal reflection
      return glm::vec3(1.0f, 1.0f, 1.0f);
    }
    // larger of incidence and transmitted angles
    cosTheta = fmin(cosTheta, sqrt(1.0 - sinThetaT * sinThetaT));
    // calculate reflectance
    glm::vec3 reflectance =
        f0_reflectance + (1.0f - f0_reflectance) * pow((1 - cosTheta), 5.0f);
    return reflectance;
  }
  glm::vec3 transmittance(const glm::vec3 normal,
                          const glm::vec3 incidentdirection) const override {

    return 1.0f - this->reflectance(normal, incidentdirection);
  }

  bool isTransparent() const override { return true; }

private:
  float refractive_index;
};
