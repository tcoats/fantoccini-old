#ifndef _fantoccini_h
#define _fantoccini_h

#include <array>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

namespace ECS {
  long NewID();
  void Bind();
}
namespace Fantoccini { void Bind(); }

enum class Message {
  // ECS
  OnEntityDeleted,

  // Main Loop
  OnInit,
  OnLoad,
  OnStart,
  OnEventDelta,
  OnPhysicsDelta,
  OnDisplayDelta,
  OnQuit,
  OnError,

  // Input
  OnKeyboard,
  OnMouseButton,
  OnCursorPos,
  OnScroll,
  OnJoystickConnected,
  OnJoystickDisconnected,

  // IO
  OnMagicaVoxelLoaded,
  OnFbxLoaded,
  OnTextLoaded,

  // housekeeping
  Length
};

enum class Procedure {
  // ECS
  DeleteEntity,

  // Main Loop
  Init,
  Quit,

  // Input
  DisableCursor,
  EnableCursor,

  // IO
  LoadMagicaVoxel,
  LoadFbx,
  LoadText,

  // OpenGL
  PrintOpenGLParams,
  CheckOpenGLShader,
  CheckOpenGLProgram,

  // housekeeping
  Length
};

namespace Bus {
  template <typename T> void On(Message t, void (*f)(long, T));
  template <typename T> void Off(Message t, void (*f)(long, T));
  template <typename T> void Emit(Message t, long id, T m);
  template <typename T> void On(Procedure p, void* (*f)(long, T));
  template <typename T> void Off(Procedure p);
  template <typename T> void* Emit(Procedure p, long id, T m);

  extern vector<void*> _listeners[];
  extern map<Procedure, void*> _procedures;

  template <typename T> void On(Message t, void (*f)(long, T)) {
    _listeners[(int)t].push_back((void*)f);
  }

  template <typename T> void Off(Message t, void (*f)(long, T)) {
    _listeners[(int)t].erase(
      remove(_listeners[(int)t].begin(), _listeners[(int)t].end(), (void*)f),
      _listeners[(int)t].end());
  }

  template <typename T> void Emit(Message t, long id, T m) {
    for (auto f : _listeners[(int)t]) ((void (*)(long, T))f)(id, m);
  }

  template <typename T> void On(Procedure p, void* (*f)(long, T)) {
    if (_procedures.find(p) != _procedures.end())
      Bus::Emit(Message::OnError, 0, "Duplicate procedure");
    _procedures.insert(pair<Procedure, void*>(p, (void*)f));
  }

  template <typename T> void Off(Procedure p) {
    if (_procedures.find(p) != _procedures.end()) _procedures.erase(p);
  }

  template <typename T> void* Emit(Procedure p, long id, T m) {
    return ((void* (*)(long, T))_procedures[p])(id, m);
  }
};

#endif
