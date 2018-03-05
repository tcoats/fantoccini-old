#include "../fantoccini.h"
//#include <btBulletDynamicsCommon.h>

namespace Bullet {
  void Bind() {
    Bus::On(Message::OnStart, +[](long id, double dt) { });
    Bus::On(Message::OnPhysicsDelta, +[](long id, double dt) { });
  }
}
