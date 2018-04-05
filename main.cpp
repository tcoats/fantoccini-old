#include "fantoccini.h"
#include "io/magicavoxel.h"
#include "io/fbx.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace glm;

namespace Fbx { void Bind(); }
namespace MagicaVoxel { void Bind(); }
namespace Bullet { void Bind(); }
namespace GLFW { void Bind(); }
namespace PhysFS { void Bind(); }
namespace State { void Bind(); }
namespace OpenGL {
  void Bind();
  namespace Checks { void Bind(); }
  namespace Shadow { void Bind(); }
}

int main(int argc, char* argv[]) {
  Bus::On(Event::OnError, +[](long id, const char* message) {
    cout << message << "\n";
  });

  Bus::On(Event::OnLoad, +[](long id, void* m) {
    Bus::Emit(Procedure::DisableCursor, 0, nullptr);

    // const time_t t = time(0);
    // long voxelid = ECS::NewID();
    // MagicaVoxel::Model* model = (MagicaVoxel::Model*)Bus::Emit(
    //   Procedure::LoadMagicaVoxel, voxelid, "outcrop.vox");
    // cout << put_time(gmtime(&t), "%FT%TZ") << " MagicaVoxel Size:"
    //   << model->voxels.size() << "\n";
    // ECS::Delete(voxelid);

    // long fbxid = ECS::NewID();
    // FbxScene* scene = (FbxScene*)Bus::Emit(Procedure::LoadFbx, fbxid, "tower.fbx");
    // cout << put_time(gmtime(&t), "%FT%TZ") << " Fbx "
    //   << "HELLO" << "\n";
    // ECS::Delete(fbxid);
  });

  ECS::Bind();
  MagicaVoxel::Bind();
  Fbx::Bind();
  Bullet::Bind();
  GLFW::Bind();
  PhysFS::Bind();
  State::Bind();
  OpenGL::Bind();
  OpenGL::Checks::Bind();
  OpenGL::Shadow::Bind();

  vector<string> arguments(argv, argv + argc);
  Bus::Emit(Procedure::Init, 0, &arguments);

  return 0;
}
