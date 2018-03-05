#include "fantoccini.h"
#include "io/magicavoxel.h"
#include "io/fbx.h"

#include <iostream>
#include <ctime>
#include <iomanip>
#include <queue>

namespace Bus {
  vector<void *> _listeners[(int)Message::Length + 1];
  map<Procedure, void *> _procedures;
}

namespace ECS {
  queue<long> _free;
  long _counter = 0;
  long NewID() {
    if (!_free.empty()) {
      auto result = _free.front();
      _free.pop();
      return result;
    }
    _counter++;
    return _counter;
  }
  void Bind() {
    Bus::On(Procedure::DeleteEntity, +[](long id, void* m) -> void* {
      _free.push(id);
      Bus::Emit(Message::OnEntityDeleted, id, nullptr);
      return nullptr;
    });
  }
}

namespace Fbx { void Bind(); }
namespace MagicaVoxel { void Bind(); }
namespace Bullet { void Bind(); }
namespace GLFW { void Bind(); }
namespace PhysFS { void Bind(); }
namespace State { void Bind(); }
namespace OpenGL { void Bind(); }
namespace OpenGLChecks { void Bind(); }

namespace Fantoccini {
  void Bind() {
    ECS::Bind();
    MagicaVoxel::Bind();
    Fbx::Bind();
    Bullet::Bind();
    GLFW::Bind();
    PhysFS::Bind();
    State::Bind();
    OpenGL::Bind();
    OpenGLChecks::Bind();

    Bus::On(Message::OnLoad, +[](long id, void* m) {
      Bus::Emit(Procedure::DisableCursor, 0, nullptr);

      // const time_t t = time(0);
      // long voxelid = ECS::NewID();
      // MagicaVoxel::Model* model = (MagicaVoxel::Model*)Bus::Emit(
      //   Procedure::LoadMagicaVoxel, voxelid, "outcrop.vox");
      // cout << put_time(gmtime(&t), "%FT%TZ") << " MagicaVoxel Size:"
      //   << model->voxels.size() << "\n";
      // Bus::Emit(Procedure::DeleteEntity, voxelid, nullptr);

      // long fbxid = ECS::NewID();
      // FbxScene* scene = (FbxScene*)Bus::Emit(Procedure::LoadFbx, fbxid, "tower.fbx");
      // cout << put_time(gmtime(&t), "%FT%TZ") << " Fbx "
      //   << "HELLO" << "\n";
      // Bus::Emit(Procedure::DeleteEntity, fbxid, nullptr);
    });
  }
}
