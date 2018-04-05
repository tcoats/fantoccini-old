#include "../fantoccini.h"
#include "fbx.h"
#include "fbx_physfs.h"
#include "physfs_stream.h"

#include <map>

using namespace std;

// https://forums.autodesk.com/t5/fbx-forum/fbxstream-implementation-a-reading-memory-stream/td-p/6665752
// https://github.com/SharpLinesTech/slt_tech_runtime/blob/master/tools/model_compiler/fbx_memory_stream.h

namespace Fbx {
  FbxManager* _manager = nullptr;
  FbxIOSettings* _ios = nullptr;
  map<long, FbxScene*> scenes;

  void Bind() {
    Bus::On(Event::OnInit, +[](long id, void* m) {
      _manager = FbxManager::Create();
      _ios = FbxIOSettings::Create(_manager, IOSROOT);
      _manager->SetIOSettings(_ios);
    });

    Bus::On(Event::OnQuit, +[](long id, void* m) {
      for (auto& kv : scenes)
        kv.second->Destroy();
      scenes.clear();
    });
    Bus::On(Event::OnEntityDeleted, +[](long id, void* m) {
      if (scenes.find(id) != scenes.end()) {
        scenes[id]->Destroy();
        scenes.erase(id);
      }
    });

    Bus::On(Procedure::LoadFbx, +[](long id, const char* path) -> void* {
      PhysFS::ifstream file(path);
      PhysFSStream stream(_manager, &file);
      auto importer = FbxImporter::Create(_manager, "");

      if (!importer->Initialize(&stream, nullptr, -1, _manager->GetIOSettings())) {
        Bus::Emit(Event::OnError, id, importer->GetStatus().GetErrorString());
        return nullptr;
      }

      auto scene = FbxScene::Create(_manager, "tower");
      importer->Import(scene);
      importer->Destroy();
      importer = nullptr;

      scenes.insert(pair<long, FbxScene*>(id, scene));
      return scene;
    });
  }
}
