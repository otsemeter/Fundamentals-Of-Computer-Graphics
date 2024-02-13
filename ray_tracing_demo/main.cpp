#include "Camera.h"
#include "Dielectric.h"
#include "Lambertian.h"
#include "Metal.h"
#include "PNGMaster.h"
#include "Ray.h"
#include "Scene.h"
#include "Sphere.h"
#include "Vector3.h"
#include <cfloat>
#include <chrono>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>



void render0();

void render();

Vector3 color(const Ray &, const Scene &, int);

void cal_color(const int &, const int &);

void random_scene();

unsigned int height = 128;
unsigned int width = 128;
int ray_num = 10;
Scene tmp_scene;

Vector3 look_from(13, 2, 3);
Vector3 look_at(0, 0, 0);
float focus_dis = (look_from - look_at).length();
Camera tmp_camera(look_from, look_at, Vector3(0, 1, 0), 30,
                  float(width) / float(height), 0.05, focus_dis, 0, 0);
PNGMaster tmp_pic(height, width);

int main() {

  std::cout << "Rendering : " << width << " - " << height << std::endl;
    render();
  return 0;
}

void random_scene() {
  tmp_scene.addObject(new Sphere(Vector3(0, -1000, 0), 1000,
                                 new Lambertian(Vector3(0.5, 0.5, 0.5))));
  tmp_scene.addObject(new Sphere(Vector3(0, 0, 0), 1, new Lambertian(Vector3(0.5, 0.7, 0.3))));
//  for (int a = -11; a < 11; ++a) {
//    for (int b = -11; b < 11; ++b) {
//        Vector3 tmp_center(a + 0.9 * ((double)rand() / RAND_MAX), 0, b + 0.9 * ((double)rand() / RAND_MAX));
//        auto *tmp_sphere =
//                new Sphere(tmp_center,  0.3 ,
//                           new Lambertian(Vector3(((double)rand() / RAND_MAX) * ((double)rand() / RAND_MAX),
//                                                  ((double)rand() / RAND_MAX) * ((double)rand() / RAND_MAX),
//                                                  ((double)rand() / RAND_MAX) * ((double)rand() / RAND_MAX))));
//        tmp_scene.addObject(tmp_sphere);
//    }
//  }
}

void render() {
  random_scene();

  auto start = std::chrono::system_clock::now();
  auto end = std::chrono::system_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  for (int i = 0; i < height; ++i) {
    if (i % 2 == 0) {
      cout << flush << '\r';
      printf("%.2lf%%", i * 100.0 / height);
      end = std::chrono::system_clock::now();
      duration =
          std::chrono::duration_cast<std::chrono::microseconds>(end - start);
      printf("\t%.2f min", (double)duration.count() /
                               std::chrono::microseconds::period::den / 60);
      printf("\tEstimated Time : %.2f min",
             (double)(height - i) / i * (double)duration.count() /
                 std::chrono::microseconds::period::den / 60);
    }
    for (int j = 0; j < (int)width; ++j) {
      cal_color(i, j);
    }
  }
  std::cout << "Generating Picture: " << width << "*" << height << endl;
  auto t =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::stringstream ss;
  ss << std::put_time(std::localtime(&t), "%F %T");
  std::string tmp_str = ss.str();

  char tmp_name[100];

  strcpy(tmp_name, tmp_str.c_str());
  strcat(tmp_name, "test1.png");

  end = std::chrono::system_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << flush << '\r' << "Render Completed - Time Consumed :"
            << (double)duration.count() /
                   std::chrono::microseconds::period::den / 60
            << "min" << endl;
  tmp_pic.genPNG(tmp_name);

}

void cal_color(const int &_i, const int &_j) {
  Vector3 tmp_color;
  for (int k = 0; k < ray_num; ++k) {
    double u = float(_j + ((double)rand() / RAND_MAX)) / float(width);
    double v = float(_i + ((double)rand() / RAND_MAX)) / float(height);
    Ray tmp_ray = tmp_camera.gen_ray(u, v);
    tmp_color += color(tmp_ray, tmp_scene, 0);
    ;
  }
  tmp_color /= ray_num;
  tmp_color = Vector3(sqrt(tmp_color.r()), sqrt(tmp_color.g()),
                      sqrt(tmp_color.b())); // gamma corrected with gamma 2
  tmp_color *= 255.99;
  tmp_pic.setPixel(_j, _i, int(tmp_color.r()), int(tmp_color.g()),
                   int(tmp_color.b()));
}

Vector3 color(const Ray &_r, const Scene &_s, int _d) {
  HitInfo tmp_info;
  if (_s.hit(_r, 0.001, DBL_MAX, tmp_info)) { // if hit object
    Ray scatter_ray;
    Vector3 attenuation_vec;
    if (_d < 20 && tmp_info.material_ptr->scatter(_r, tmp_info, attenuation_vec,
                                                  scatter_ray)) {
      return attenuation_vec * color(scatter_ray, _s, _d + 1);
    } else {
      return Vector3(0, 0, 0);
    }
  } else {
    Vector3 unit_vec = _r.direction();
    double t = 0.5 * (unit_vec.y() + 1.0);
    return (1.0 - t) * Vector3(1, 1, 1) + t * Vector3(0.5, 0.7, 1.0);
  }
}
