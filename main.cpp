#include "fantoccini.h"
#include "io/magicavoxel.h"
#include "io/fbx.h"

#include <iostream>
#include <ctime>
#include <iomanip>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

namespace Fbx { void Bind(); }
namespace MagicaVoxel { void Bind(); }
namespace Bullet { void Bind(); }
namespace GLFW { void Bind(); }
namespace PhysFS { void Bind(); }
namespace OpenGL {
  void Bind();
  namespace Checks { void Bind(); }
  namespace Shadow { void Bind(); }
}

int main(int argc, char* argv[]) {
  Bus::On(Event::OnError, +[](long id, const char* message) {
    cout << message << "\n";
  });

  MagicaVoxel::Bind();
  Fbx::Bind();
  Bullet::Bind();
  GLFW::Bind();
  PhysFS::Bind();
  OpenGL::Bind();
  OpenGL::Checks::Bind();
  OpenGL::Shadow::Bind();

  auto cameraId = ECS::NewID();
  auto cameraPosition = vec3(0.0f, 0.0f, 5.0f);
  auto cameraOrientation = quat(vec3(0.0f, 1.0f * pi<float>(), 0.0f));
  auto cameraProjection = perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.f);
  ECS::Add(cameraId, Component::Position, &cameraPosition);
  ECS::Add(cameraId, Component::Orientation, &cameraOrientation);
  ECS::Add(cameraId, Component::Projection, &cameraProjection);
  Bus::Emit(Event::OnCamera, cameraId, nullptr);

  Bus::On(Event::OnLoad, +[](long id, void* m) {
    Bus::Emit(Procedure::DisableCursor, 0, nullptr);
  });

  vector<string> arguments(argv, argv + argc);
  Bus::Emit(Procedure::Init, 0, &arguments);

  ECS::Delete(cameraId);

  return 0;
}
