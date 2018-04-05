#include "../fantoccini.h"
//#include <btBulletDynamicsCommon.h>

namespace Bullet {
  void Bind() {
    Bus::On(Event::OnStart, +[](long id, double dt) { });
    Bus::On(Event::OnPhysicsDelta, +[](long id, double dt) { });
  }
}
