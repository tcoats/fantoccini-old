#ifndef _magicavoxel_h
#define _magicavoxel_h

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// const time_t t = time(0);
// auto voxelid = ECS::NewID();
// auto model = (MagicaVoxel::Model*)Bus::Emit(
//   Procedure::LoadMagicaVoxel, voxelid, "outcrop.vox");
// cout << put_time(gmtime(&t), "%FT%TZ") << " MagicaVoxel Size:"
//   << model->voxels.size() << "\n";
// ECS::Delete(voxelid);

// https://github.com/ephtracy/voxel-model/blob/master/MagicaVoxel-file-format-vox.txt
namespace MagicaVoxel {
  using namespace glm;
  using namespace std;

  struct Model {
    vec3 dimensions;
    vector<vec4> voxels;
    vec4 palette[256];
  };
}

#endif
