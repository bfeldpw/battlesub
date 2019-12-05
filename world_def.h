#ifndef WORLD_DEF_H
#define WORLD_DEF_H

#include <Magnum/GL/Texture.h>

constexpr float WORLD_SIZE_X = 1024.0f;
constexpr float WORLD_SIZE_Y = 512.0f;

using namespace Magnum;

constexpr GL::SamplerFilter FLUID_GRID_DIFFUSION_FILTER = GL::SamplerFilter::Linear;
constexpr GL::SamplerMipmap FLUID_GRID_DIFFUSION_FILTER_MIP_MAP = GL::SamplerMipmap::Base;

constexpr int FLUID_GRID_SIZE_X_BITS = 11;
constexpr int FLUID_GRID_SIZE_X = 1 << FLUID_GRID_SIZE_X_BITS;
constexpr int FLUID_GRID_SIZE_Y = 1 << (FLUID_GRID_SIZE_X_BITS-1);
constexpr int FLUID_GRID_ARRAY_SIZE = FLUID_GRID_SIZE_X * FLUID_GRID_SIZE_Y;

#endif // WORLD_DEF_H
