#include "../fantoccini.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

namespace OpenGLChecks {
  void Bind() {
    Bus::On(Procedure::PrintOpenGLParams, +[](long id, void* m) -> void* {
      cout << "{\n";
      cout << "  \"GL Version\": \""
        << glGetString(GL_VERSION) << "\",\n";
      cout << "  \"GLSL Version\": \""
        << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\",\n";
      cout << "  \"Vendor\": \""
        << glGetString(GL_VENDOR) << "\",\n";
      cout << "  \"Renderer\": \""
        << glGetString(GL_RENDERER) << "\",\n";

      cout << "  \"Maximums\": {\n";
      int a = 0;
      glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &a);
      cout << "    \"Combined texture image units\": " << a << ",\n";
      glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &a);
      cout << "    \"Cube map texture size\": " << a << ",\n";
      glGetIntegerv(GL_MAX_DRAW_BUFFERS, &a);
      cout << "    \"Draw buffers\": " << a << ",\n";
      glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &a);
      cout << "    \"Fragment uniform components\": " << a << ",\n";
      glGetIntegerv(GL_MAX_VARYING_FLOATS, &a);
      cout << "    \"Max varying floats\": " << a << "\n";

      cout << "    \"Texture\": {\n";
      glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &a);
      cout << "      \"Image units\": " << a << ",\n";
      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &a);
      cout << "      \"Size\": " << a << "\n";
      cout << "     },\n";

      cout << "    \"Vertex\": {\n";
      glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &a);
      cout << "      \"Attributes\": " << a << ",\n";
      glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &a);
      cout << "      \"Texture image units\": " << a << ",\n";
      glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &a);
      cout << "      \"Uniform components\": " << a << "\n";
      cout << "     },\n";
      int b[2] = { 0, 0 };
      glGetIntegerv(GL_MAX_VIEWPORT_DIMS, b);
      cout << "    \"Viewport dimensions\": ["
        << b[0] << ", " << b[1] << "],\n";
      unsigned char c = 0;
      glGetBooleanv(GL_STEREO, &c);
      cout << "    \"Stereo\": " << (c == 0 ? "false" : "true") << "\n";
      cout << "  }\n";
      cout << "}\n";
      return NULL;
    });

    Bus::On(Procedure::CheckOpenGLShader, +[](long id, unsigned int* shaderId) -> void* {
      GLint status;
      glGetShaderiv(*shaderId, GL_COMPILE_STATUS, &status);
      if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(*shaderId, GL_INFO_LOG_LENGTH, &length);
        vector<char> log(length);
        glGetShaderInfoLog(*shaderId, length, &length, &log[0]);
        cerr << &log[0];
        Bus::Emit(Procedure::Quit, 0, NULL);
        return (void*)false;
      }
      return (void*)true;
    });

    Bus::On(Procedure::CheckOpenGLProgram, +[](long id, unsigned int* programId) -> void* {
      GLint status;
      glGetProgramiv(*programId, GL_LINK_STATUS, &status);
      if (status == GL_FALSE) {
        GLint length;
        glGetProgramiv(*programId, GL_INFO_LOG_LENGTH, &length);
        vector<char> log(length);
        glGetProgramInfoLog(*programId, length, &length, &log[0]);
        cerr << &log[0];
        Bus::Emit(Procedure::Quit, 0, NULL);
        return (void*)false;
      }
      return (void*)true;
    });
  }
}
