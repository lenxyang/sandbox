#pragma once

#include "azer/math/math.h"
#include "azer/render/render.h"

struct Material {
  azer::Vector4 diffuse_color;
  azer::Vector4 specular_color;
  float specular_power;
  float specular_intensity;
};
