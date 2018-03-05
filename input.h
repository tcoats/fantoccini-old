#ifndef _input_h
#define _input_h

namespace Input {
  struct KeyboardEvent {
    int key, scancode, action, mods;
  };

  struct MouseButtonEvent {
    int button, action, mods;
  };
}

#endif
