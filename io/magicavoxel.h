#ifndef _magicavoxel_h
#define _magicavoxel_h

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt
namespace MagicaVoxel {
  struct Model {
    glm::vec3 dimensions;
    std::vector<glm::vec4> voxels;
    glm::vec4 palette[256];
  };
}

#endif
