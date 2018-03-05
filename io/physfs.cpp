#include <streambuf>
#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <physfs.h>

#include "../fantoccini.h"
#include "state.h"

using namespace std;

namespace PhysFS {
  map<long, char*> files;

  void Bind() {
    Bus::On(Message::OnQuit, +[](long id, void* m) {
      for (auto& kv : files)
        delete kv.second;
      files.clear();
      PHYSFS_deinit();
    });
    Bus::On(Message::OnEntityDeleted, +[](long id, void* m) {
      if (files.find(id) != files.end()) {
        delete[] files[id];
        files.erase(id);
      }
    });

    Bus::On(Message::OnInit, +[](long id, vector<string>* arguments) {
      PHYSFS_init((*arguments)[0].c_str());
      auto dataDirectory = State::GetDataDirectory();
      PHYSFS_setWriteDir(dataDirectory.c_str());
      PHYSFS_mount(dataDirectory.c_str(), nullptr, 1);
      string resourceDirectory = "./resources/";
      #ifdef __APPLE__
      resourceDirectory = "../Resources/";
      #endif
      PHYSFS_mount(resourceDirectory.c_str(), nullptr, 1);
      ifstream manifest("manifest.txt");
      vector<string> paths;
      string line;
      while (getline(manifest, line)) paths.push_back(line);
      for (auto path : paths)
        PHYSFS_mount((resourceDirectory + path).c_str(), nullptr, 1);

      cout << "{\n";
      cout << "  \"State\": \"" << dataDirectory << "\",\n";
      cout << "  \"Search\": [\n";
      cout << "    \"" << dataDirectory << "\"";
      cout << ",\n    \"" << resourceDirectory.c_str() << "\"";
      for (auto path : paths)
        cout << ",\n" << "    \"" << (resourceDirectory + path).c_str() << "\"";
      cout << "\n  ]\n}\n";
    });

    Bus::On(Procedure::LoadText, +[](long id, const char* path) -> void* {
      auto file = PHYSFS_openRead(path);
      auto size = PHYSFS_fileLength(file);
      auto content = new char[size + 1];
      PHYSFS_readBytes(file, content, size);
      PHYSFS_close(file);
      content[size] = '\0';
      file = nullptr;
      files.insert(pair<long, char*>(id, content));
      cout << path << ": " << size << " bytes\n";
      //cout << content << endl;
      Bus::Emit(Message::OnTextLoaded, id, content);
      return content;
    });
  }
}
