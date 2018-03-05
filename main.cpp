#include "fantoccini.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
  Bus::On(Message::OnError, +[](long id, const char* message) {
    cout << message << "\n";
  });

  Fantoccini::Bind();

  vector<string> arguments(argv, argv + argc);
  Bus::Emit(Procedure::Init, 0, &arguments);

  return 0;
}
