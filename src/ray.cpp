#include "ray.hpp"
#include <iostream>
#include <math.h>
#include <random>
#include <cstdlib>
#include <ctime>

using namespace std; 

double doubleRand() {
  return double(rand()) / (double(RAND_MAX) + 1.0);
}
#define PI 3.14159265358979323846f

RayTracer::RayTracer(const int width, const int height, const float vfov, const glm::vec3& camera_position, const glm::vec3& camera_direction, const glm::vec3& camera_up):
  width(width), height(height), 
  vfov(vfov * PI / 180), 
  hfov(2 * atan(tan(vfov / 2 * PI / 180) * width / height)), 
  camera_position(camera_position), camera_direction(camera_direction), camera_up(camera_up)
{
  framebuffer = new glm::uvec3*[width];
  for (int i = 0; i < width; i++) {
    framebuffer[i] = new glm::uvec3[height];
  }
}

  glm::uvec3** RayTracer::render(const std::vector<Shape*>& shapes) {
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      glm::vec3 color_sum(0.0f);
      float num_samples=10;
      for (int k = 0; k < num_samples; k++) {
        float du=static_cast<float>(rand()) / RAND_MAX;
        float dv=static_cast<float>(rand()) / RAND_MAX;
        //du=0;dv=0;
        float u = (i + du) / (width - 1);
        float v = (j + dv) / (height - 1);
        //std::cout<<du<<" "<<dv<<"\n";
        glm::vec3 vertical = camera_up;
        glm::vec3 horizontal = glm::normalize(glm::cross(camera_direction, vertical));
        float v_w = (2 * v - 1) * tan(vfov / 2);
        float h_w = (2 * u - 1) * tan(hfov / 2);
        glm::vec3 d = glm::normalize(camera_direction + h_w * horizontal + v_w * vertical);
        color_sum += trace(camera_position, d, shapes, 0);
      }
      framebuffer[i][j] = glm::uvec3(255.0f * (color_sum / num_samples));
    }
  }
  // normalize the framebuffer if any value is greater than 255
  uint32_t max = 0;
  uint32_t avg = 0;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      if (framebuffer[i][j].x > max) max = framebuffer[i][j].x;
      if (framebuffer[i][j].y > max) max = framebuffer[i][j].y;
      if (framebuffer[i][j].z > max) max = framebuffer[i][j].z;
      avg+=(framebuffer[i][j].x+framebuffer[i][j].y+framebuffer[i][j].z)/3;
    }
  }
  avg/=width*height;
  std::cout<<max<<"\n";
  std::cout<<avg<<"\n";
  //max=std::fmin(max,2000);
  if (max > 255) {
    for (int i = 0; i < width; i++) {
      for (int j = 0; j < height; j++) {
        framebuffer[i][j] = framebuffer[i][j] * 255u / max;
      }
    }
  }
  return framebuffer;
}

void RayTracer::shoot_ray(const glm::vec3& o, const glm::vec3& d, const std::vector<Shape*>& shapes, float& hit_t, glm::vec3& hit_normal, Shape*& hit_shape) const{
  // find the closest shape and its intersection point
  Shape* closest_shape = nullptr;
  glm::vec3 closest_position, closest_normal;
  float closest_t = std::numeric_limits<float>::max();
  
  // intersect with all shapes
  for(Shape* shape : shapes){
    glm::vec3 temp_normal;
    float t = shape->intersect(o, d, T_MIN, T_MAX, temp_normal);
    //std::cout<<"t: "<<t<<"\n";
    if(t > T_MIN && t < closest_t && t < T_MAX){
      closest_t = t;
      closest_shape = shape;
      closest_normal = temp_normal;
    }
  }

  if (closest_shape != nullptr) {
    hit_t = closest_t;
    hit_normal = closest_normal;
    hit_shape = closest_shape;
  }
  else{
    hit_shape = nullptr;
  }
}

bool RayTracer::shadow(const glm::vec3& o, const Shape* light, const std::vector<Shape*>& shapes) const{
  // check if the ray from o to light->position intersects any shape
  glm::vec3 d = glm::normalize(light->get_position() - o);
  // using shoot_ray
  float closest_t;
  glm::vec3 closest_normal;
  Shape* closest_shape;
  shoot_ray(o, d, shapes, closest_t, closest_normal, closest_shape);
  if(closest_shape != nullptr){
    if(closest_shape != light){
      return true;
    }
  }
  return false;
}

glm::vec3 RayTracer::SampleDiffuse(const glm::vec3& normal) const
{
    float u1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float u2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    float r = std::sqrt(u1);
    float theta = 2.0f * PI * u2;

    glm::vec3 w = normal;
    glm::vec3 u = glm::normalize(glm::cross((std::abs(w.x) > 0.1f ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0)), w));
    glm::vec3 v = glm::cross(w, u);

    glm::vec3 direction = glm::normalize(u * std::cos(theta) * r + v * std::sin(theta) * r + w * std::sqrt(1.0f - u1));

    return direction;
}

glm::vec3 RayTracer::trace(const glm::vec3& o, const glm::vec3& d, const std::vector<Shape*>& shapes, int recursion_depth) const{

  //Parameters
  float c1=1,c2=0.8;  // for light attenuation
  double expected_number_of_bounces=20.0;
  double continuation_probability=1.0f-1.0/expected_number_of_bounces;
  // find the closest shape and its intersection point using shoot_ray
  float closest_t;
  glm::vec3 closest_normal;
  Shape* closest_shape;
  shoot_ray(o, d, shapes, closest_t, closest_normal, closest_shape);
  glm::vec3 closest_position = o + closest_t * d;\
  float min_distance=0.1;
  // if there is no intersection, return the background color
  if(closest_shape == nullptr||recursion_depth>=10){
    return glm::vec3(0.0f);  //return black
  }

  vector<Shape*> lights;
  for(Shape* shape : shapes){
    if(shape->is_light){
      lights.push_back(shape);
    }
  }

  // if there are no lights, use the default scheme
  if(lights.size() == 0){
    return (closest_normal + glm::vec3(1.0f)) / 2.0f;
  }
  else{
    // check if the shape is a light
    if(!closest_shape->material->is_metallic&&!closest_shape->material->is_transparent){
    for(Shape* light : lights){
      if(closest_shape == light){
        glm::vec3 l = light->get_position() - closest_position;
        return light->albedo*light->intensity / (c1+c2*glm::dot(l, l));
      }
    }
    // not a light
    glm::vec3 l_i(0.0f);
    for(Shape* light : lights){
      glm::vec3 l = light->get_position() - closest_position;
      float distance = glm::length(l);
      l = l / distance;
      //std::cout<<distance<<"\n";
      //distance=max(distance,min_distance);
      if(!shadow(closest_position, light, shapes)){
        // flux = I * dA cos(theta) / r^2
        // irradiance = flux / d
        // radiance = brdf * irradiance
        glm::vec3 e = light->albedo*light->intensity * glm::max(glm::dot(l, closest_normal), 0.0f) / (c1+c2*(distance * distance));
        l_i += closest_shape->albedo / PI * e; 
      }
    }

    // Add cosine weighted sampling
    glm::vec3 path_ray = SampleDiffuse(closest_normal);
    if(doubleRand()<continuation_probability){
      l_i+=trace(closest_position+T_MIN*path_ray,path_ray,shapes,recursion_depth+1);
    }
    
    return l_i;
    }
      // Shape is metallic)
      if(closest_shape->material->is_metallic){
        for(Shape* light : lights){
            if(closest_shape == light){
                glm::vec3 l = light->get_position() - closest_position;
            return light->albedo*light->intensity / (c1+c2*glm::dot(l, l));
          }
        }
        glm::vec3 l_ii(0.0f);
        glm::vec3 reflection_dir=closest_shape->material->reflect(closest_normal,d);
        glm::vec3 reflectance=closest_shape->material->reflectance(closest_normal,d);

        for(Shape* light : lights){
        glm::vec3 l = light->get_position() - closest_position;
        float distance = glm::length(l);
        l = l / distance;
        //std::cout<<distance<<"\n";
        //distance=max(distance,min_distance);
        if(!shadow(closest_position, light, shapes)){
        // flux = I * dA cos(theta) / r^2
        // irradiance = flux / d
        // radiance = brdf * irradiance
        glm::vec3 e = light->albedo*light->intensity * glm::max(glm::dot(l, reflection_dir), 0.0f) / (c1+c2*(distance * distance));
        l_ii += reflectance*e/PI; 
      }
    }
        //std::cout<<glm::dot(d,closest_normal)<<"\n";
        //glm::vec3 reflection_dir=closest_shape->material->reflect(closest_normal,d);
        l_ii+=reflectance*trace(closest_position+T_MIN*reflection_dir,reflection_dir,shapes,recursion_depth+1); //with reflected ray
        return l_ii;
      }
      // Shape is transparent
      else if(closest_shape->material->is_transparent){

            for(Shape* light : lights){
              if(closest_shape == light){
                  glm::vec3 l = light->get_position() - closest_position;
                  return light->albedo*light->intensity / (c1+c2*glm::dot(l, l));
              }
            }
            // Reflect and refract
            glm::vec3 l_iii(0.0f);
            glm::vec3 reflection_dir=closest_shape->material->reflect(closest_normal,d);
            glm::vec3 reflectans=closest_shape->material->reflectance(closest_normal,d);
            glm::vec3 transmittans=1.0f-reflectans;
            glm::vec3 transmitted_dir = closest_shape->material->transmit(closest_normal, d);

            for(Shape* light : lights){
            glm::vec3 l = light->get_position() - closest_position;
            float distance = glm::length(l);
            l = l / distance;
            //std::cout<<distance<<"\n";
            //distance=max(distance,min_distance);
            if(!shadow(closest_position, light, shapes)){
          // flux = I * dA cos(theta) / r^2
          // irradiance = flux / d
            // radiance = brdf * irradiance
            glm::vec3 e = light->albedo*light->intensity * glm::max(glm::dot(l, reflection_dir), 0.0f) / (c1+c2*(distance * distance));
            l_iii += reflectans / PI * e; 
            if(transmitted_dir==reflection_dir){
              glm::vec3 e = light->albedo*light->intensity * glm::max(glm::dot(l, reflection_dir), 0.0f) / (c1+c2*(distance * distance));
              l_iii += transmittans / PI * e; 
            }
            else{
              glm::vec3 e = light->albedo*light->intensity * glm::max(glm::dot(l,-transmitted_dir), 0.0f) / (c1+c2*(distance * distance));
            l_iii += transmittans / PI * e; 
            }
              }
          }
            l_iii+=transmittans*trace(closest_position+T_MIN*transmitted_dir,transmitted_dir,shapes,recursion_depth+1); //with refracted ray
            glm::vec3 reflected_dir = closest_shape->material->reflect(closest_normal, d);
            l_iii+=reflectans*trace(closest_position+T_MIN*reflected_dir,reflected_dir,shapes,recursion_depth+1); //with reflected ray
            return l_iii;

          }
  }
}
