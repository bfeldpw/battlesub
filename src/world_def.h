#ifndef WORLD_DEF_H
#define WORLD_DEF_H

#include <Magnum/GL/Texture.h>

constexpr float WORLD_SIZE_DEFAULT_X = 512.0f;
constexpr float WORLD_SIZE_DEFAULT_Y = 256.0f;

constexpr int WINDOW_RESOLUTION_MAX_X = 4096;
constexpr int WINDOW_RESOLUTION_MAX_Y = 2048;

using namespace Magnum;

constexpr GL::SamplerFilter FLUID_GRID_DIFFUSION_FILTER = GL::SamplerFilter::Linear;
constexpr GL::SamplerMipmap FLUID_GRID_DIFFUSION_FILTER_MIP_MAP = GL::SamplerMipmap::Linear;

constexpr int FLUID_GRID_SIZE_X_BITS = 11; // = 2048 => 0.25m
constexpr int FLUID_GRID_SIZE_X = 1 << FLUID_GRID_SIZE_X_BITS;
constexpr int FLUID_GRID_SIZE_Y = 1 << (FLUID_GRID_SIZE_X_BITS-1);
constexpr int FLUID_GRID_ARRAY_SIZE = FLUID_GRID_SIZE_X * FLUID_GRID_SIZE_Y;

#endif // WORLD_DEF_H
