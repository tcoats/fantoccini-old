#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <iostream>

#include "../fantoccini.h"
#include "../input.h"

using namespace glm;
using namespace std;

GLFWwindow* window;

namespace GLFW {
  GLFWwindow* _window;
  auto _targetresolution = ivec2(1920, 1080);
  long _joysticks[GLFW_JOYSTICK_LAST + 1];

  void Bind() {
    Bus::On(Message::OnEventDelta, +[](long id, double dt) {
      glfwPollEvents();
      if (GLFW_PRESS == glfwGetKey(_window, GLFW_KEY_ESCAPE)) {
        Bus::Emit(Procedure::Quit, 0, nullptr);
        return;
      }
      for (int i = 0; i <= GLFW_JOYSTICK_LAST; i++) {
        if (glfwJoystickPresent(i) == GLFW_TRUE) {
          if (_joysticks[i] != 0) continue;
          auto id = ECS::NewID();
          _joysticks[i] = id;
          Bus::Emit(Message::OnJoystickConnected, id, nullptr);
        }
        else {
          if (_joysticks[i] == 0) continue;
          Bus::Emit(Message::OnJoystickDisconnected, _joysticks[i], nullptr);
          _joysticks[i] = 0;
        }
      }
    });

    Bus::On(Message::OnQuit, +[](long id, void* m) {
      glfwTerminate();
    });

    Bus::On(Procedure::Quit, +[](long id, void* m) -> void* {
      glfwSetWindowShouldClose(_window, true);
      return nullptr;
    });

    Bus::On(Procedure::DisableCursor, +[](long id, void* m) -> void* {
      glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      return nullptr;
    });

    Bus::On(Procedure::EnableCursor, +[](long id, void* m)  -> void* {
      glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      return nullptr;
    });

    Bus::On(Procedure::Init, +[](long id, void* m) -> void* {
      Bus::Emit(Message::OnInit, 0, m);
      Bus::Emit(Message::OnLoad, 0, m);
      Bus::Emit(Message::OnStart, 0, m);
      double lastFrame = 0.0f;
      while (!glfwWindowShouldClose(_window)) {
        auto currentFrame = glfwGetTime();
        auto delta = currentFrame - lastFrame;
        lastFrame = currentFrame;
        Bus::Emit(Message::OnEventDelta, 0, delta);
        Bus::Emit(Message::OnPhysicsDelta, 0, delta);
        glViewport(0, 0, _targetresolution.x, _targetresolution.y);
        Bus::Emit(Message::OnDisplayDelta, 0, delta);
        glfwSwapBuffers(_window);
      }
      Bus::Emit(Message::OnQuit, 0, nullptr);
      return nullptr;
    });

    Bus::On(Message::OnInit, +[](long id, void* m) {
      glfwSetErrorCallback(+[](int error, const char* description) {
        Bus::Emit(Message::OnError, 0, description);
      });
      if (!glfwInit())
      {
        Bus::Emit(Message::OnError, 0, "glfwInit");
        Bus::Emit(Procedure::Quit, 0, nullptr);
      }

      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
      glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
      glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
      glfwWindowHint(GLFW_SAMPLES, 4);

      _window = glfwCreateWindow(_targetresolution.x, _targetresolution.y, "Fantoccini", nullptr, nullptr);
      window = _window;
      if (!_window)
      {
        Bus::Emit(Message::OnError, 0, "glfwCreateWindow");
        Bus::Emit(Procedure::Quit, 0, nullptr);
        glfwTerminate();
        return;
      }

      // scale screen _targetresolution to get desired pixel _targetresolution
      ivec2 current;
      glfwGetFramebufferSize(_window, &current.x, &current.y);
      if (current.x != _targetresolution.x || current.y != _targetresolution.y)
        glfwSetWindowSize(_window,
          (_targetresolution.x / (float)current.x) * _targetresolution.x,
          (_targetresolution.y / (float)current.y) * _targetresolution.y);

      glfwSetWindowPos(_window, 0, 0);
      glfwMakeContextCurrent(_window);

      glewExperimental = GL_TRUE;
      if (glewInit() != GLEW_OK) {
        Bus::Emit(Message::OnError, 0, "glewInit");
        Bus::Emit(Procedure::Quit, 0, nullptr);
        glfwTerminate();
        return;
      }

      Bus::Emit(Procedure::PrintOpenGLParams, 0, nullptr);

      glfwSetKeyCallback(_window,
        +[](GLFWwindow* window, int key, int scancode, int action, int mods) {
        Input::KeyboardEvent e;
        e.key = key;
        e.scancode = scancode;
        e.action = action;
        e.mods = mods;
        Bus::Emit(Message::OnKeyboard, 0, &e);
      });
      glfwSetMouseButtonCallback(_window,
        +[](GLFWwindow* window, int button, int action, int mods) {
        Input::MouseButtonEvent e;
        e.button = button;
        e.action = action;
        e.mods = mods;
        Bus::Emit(Message::OnMouseButton, 0, &e);
      });
      glfwSetCursorPosCallback(_window,
        +[](GLFWwindow* window, double xpos, double ypos) {
        vec2 e(xpos, ypos);
        Bus::Emit(Message::OnCursorPos, 0, &e);
      });
      glfwSetScrollCallback(_window,
        +[](GLFWwindow* window, double xoffset, double yoffset) {
        vec2 e(xoffset, yoffset);
        Bus::Emit(Message::OnScroll, 0, &e);
      });
    });
  }
}
