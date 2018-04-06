#include "../fantoccini.h"
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace glm;

namespace OpenGL {
  vec3* cameraPosition;
  quat* cameraOrientation;
  mat4* cameraProjection;
  mat4 worldTransform;

  auto lightDirection = normalize(vec3(-0.5f, 1.0f, 0.25f));
  auto depthProjection = ortho(
    -10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 20.0f);
  auto depthTransform = lookAt(
    lightDirection,
    vec3(0, 0, 0),
    vec3(0, 1, 0));
  auto depthBias = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
  );
  auto worldDepthTransform = depthProjection * depthTransform;
  auto depthBiasTransform = depthBias * worldDepthTransform;

  unsigned int vertexArrayId;
  unsigned int verticiesId;
  unsigned int indiciesId;
  unsigned int instancesId;
  unsigned int frameBufferId;
  unsigned int shadowTextureId;
  unsigned int quadVertexArrayId;
  unsigned int quadVerticiesId;

  struct modelShaderRef {
    unsigned int id;

    int worldTransform;
    int depthBiasTransform;
    int shadowMap;
    int lightDirection;
    int position;
    int normal;
    int modelTransform;
    int color;
  };
  modelShaderRef modelShader;

  struct shadowShaderRef {
    unsigned int id;

    int worldDepthTransform;
    int position;
    int modelTransform;
  };
  shadowShaderRef shadowShader;

  struct quadShaderRef {
    unsigned int id;

    int shadowMap;
    int position;
  };
  quadShaderRef quadShader;

  struct instance {
    instance(mat4 modelTransform, vec3 color)
      : modelTransform(modelTransform), color(color) {
    }
    mat4 modelTransform;
    vec3 color;
  };

  struct vertex {
    vertex(vec3 position, vec3 normal)
      : position(position), normal(normal) {
    }
    vec3 position;
    vec3 normal;
  };

  vertex verticies[] = {
    // face 0:
    vertex(vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
    vertex(vec3(-1.0f, 1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
    vertex(vec3(1.0f, -1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),
    vertex(vec3(-1.0f, -1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f)),

    // face 1:
    vertex(vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),
    vertex(vec3(1.0f, -1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f)),
    vertex(vec3(1.0f, 1.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f)),
    vertex(vec3(1.0f, -1.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f)),

    // face 2:
    vertex(vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
    vertex(vec3(1.0f, 1.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f)),
    vertex(vec3(-1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f)),
    vertex(vec3(-1.0f, 1.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f)),

    // face 3:
    vertex(vec3(1.0f, 1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f)),
    vertex(vec3(1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f)),
    vertex(vec3(-1.0f, 1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f)),
    vertex(vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, 0.0f, -1.0f)),

    // face 4:
    vertex(vec3(-1.0f, 1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
    vertex(vec3(-1.0f, 1.0f, -1.0f), vec3(-1.0f, 0.0f, 0.0f)),
    vertex(vec3(-1.0f, -1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f)),
    vertex(vec3(-1.0f, -1.0f, -1.0f), vec3(-1.0f, 0.0f, 0.0f)),

    // face 5:
    vertex(vec3( 1.0f, -1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
    vertex(vec3(-1.0f, -1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f)),
    vertex(vec3( 1.0f, -1.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f)),
    vertex(vec3(-1.0f, -1.0f, -1.0f), vec3(0.0f, -1.0f, 0.0f))
  };
  auto verticies_count = sizeof(verticies) / sizeof(vertex);

  ivec3 indicies[] = {
    // face 0:
    ivec3(0, 1, 2),
    ivec3(2, 1, 3),
    // face 1:
    ivec3(4, 5, 6),
    ivec3(6, 5, 7),
    // face 2:
    ivec3(8, 9, 10),
    ivec3(10, 9, 11),
    // face 3:
    ivec3(12, 13, 14),
    ivec3(14, 13, 15),
    // face 4:
    ivec3(16, 17, 18),
    ivec3(18, 17, 19),
    // face 5:
    ivec3(20, 21, 22),
    ivec3(22, 21, 23)
  };
  auto indicies_count = sizeof(indicies) / sizeof(int);
  auto indicies_triangles = sizeof(indicies) / sizeof(ivec3);

  instance instances[] = {
    instance(
      translate(mat4(), vec3(2.0f, 2.0f, 2.0f)),
      vec4(1.0f, 0.0f, 0.0f, 1.0f)
    ),
    instance(
      translate(mat4(), vec3(2.0f, 2.0f, -2.0f)),
      vec4(0.0f, 1.0f, 0.0f, 1.0f)
    ),
    instance(
      translate(mat4(), vec3(2.0f, -2.0f, 2.0f)),
      vec4(0.0f, 0.0f, 1.0f, 1.0f)
    ),
    instance(
      translate(mat4(), vec3(2.0f, -2.0f, -2.0f)),
      vec4(1.0f, 1.0f, 0.0f, 1.0f)
    ),
    instance(
      translate(mat4(), vec3(-2.0f, 2.0f, 2.0f)),
      vec4(1.0f, 0.0f, 1.0f, 1.0f)
    ),
    instance(
      translate(mat4(), vec3(-2.0f, 2.0f, -2.0f)),
      vec4(0.0f, 1.0f, 1.0f, 1.0f)
    ),
    instance(
      translate(mat4(), vec3(-2.0f, -2.0f, 2.0f)),
      vec4(1.0f, 0.5f, 0.5f, 1.0f)
    ),
    instance(
      translate(mat4(), vec3(-2.0f, -2.0f, -2.0f)),
      vec4(0.5f, 1.0f, 0.5f, 1.0f)
    )
  };
  auto instances_count = sizeof(instances) / sizeof(instance);

  GLfloat quadVerticies[] = {
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
  };

  unsigned int LoadProgram(const string path) {
    auto vertId = glCreateShader(GL_VERTEX_SHADER);
    auto vertSourceId = ECS::NewID();
    auto vertSource = (const char*)Bus::Emit(
      Procedure::LoadText, vertSourceId, (path + ".vert").c_str());
    glShaderSource(vertId, 1, &vertSource, nullptr);
    glCompileShader(vertId);
    Bus::Emit(Procedure::CheckOpenGLShader, 0, &vertId);
    auto fragId = glCreateShader(GL_FRAGMENT_SHADER);
    auto fragSourceId = ECS::NewID();
    auto fragSource = (const char*)Bus::Emit(
      Procedure::LoadText, fragSourceId, (path + ".frag").c_str());
    glShaderSource(fragId, 1, &fragSource, nullptr);
    glCompileShader(fragId);
    Bus::Emit(Procedure::CheckOpenGLShader, 0, &fragId);
    auto result = glCreateProgram();
    glAttachShader(result, fragId);
    glAttachShader(result, vertId);
    glLinkProgram(result);
    Bus::Emit(Procedure::CheckOpenGLProgram, 0, &result);
    glDetachShader(result, fragId);
    glDetachShader(result, vertId);
    glDeleteShader(fragId);
    glDeleteShader(vertId);
    ECS::Delete(vertSourceId);
    ECS::Delete(fragSourceId);
    return result;
  }

  void Bind() {
    Bus::On(Event::OnCamera, +[](long id, void* m) {
      cameraPosition = ECS::Get<vec3*>(id, Component::Position);
      cameraOrientation = ECS::Get<quat*>(id, Component::Orientation);
      cameraProjection = ECS::Get<mat4*>(id, Component::Projection);
      worldTransform = (*cameraProjection)
        * mat4_cast(*cameraOrientation)
        * translate(mat4(1.0f), *cameraPosition);
    });

    Bus::On(Event::OnLoad, +[](long id, void* m) {
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LESS);
      glEnable(GL_CULL_FACE);

      cout << "{\n";
      cout << "  \"Verticies Size\": " << sizeof(verticies) << ",\n";
      cout << "  \"Verticies Count\": " << verticies_count << ",\n";
      cout << "  \"Indicies Size\": " << sizeof(indicies) << ",\n";
      cout << "  \"Indicies Count\": " << indicies_count << ",\n";
      cout << "  \"Indicies Triangles\": " << indicies_triangles << ",\n";
      cout << "  \"Instance Size\": " << sizeof(instance) << ",\n";
      cout << "  \"Instances Size\": " << sizeof(instances) << ",\n";
      cout << "  \"Instances Count\": " << instances_count << "\n";
      cout << "}\n";

      shadowShader.id = LoadProgram("shadow");
      shadowShader.worldDepthTransform =
        glGetUniformLocation(shadowShader.id, "worldDepthTransform");
      shadowShader.modelTransform =
        glGetAttribLocation(shadowShader.id, "modelTransform");
      shadowShader.position = glGetAttribLocation(shadowShader.id, "position");

      modelShader.id = LoadProgram("flat");
      modelShader.worldTransform =
        glGetUniformLocation(modelShader.id, "worldTransform");
      modelShader.depthBiasTransform =
        glGetUniformLocation(modelShader.id, "depthBiasTransform");
      modelShader.shadowMap =
        glGetUniformLocation(modelShader.id, "shadowMap");
      modelShader.lightDirection =
        glGetUniformLocation(modelShader.id, "lightDirection");
      modelShader.modelTransform =
        glGetAttribLocation(modelShader.id, "modelTransform");
      modelShader.position = glGetAttribLocation(modelShader.id, "position");
      modelShader.normal = glGetAttribLocation(modelShader.id, "normal");
      modelShader.color = glGetAttribLocation(modelShader.id, "color");

      quadShader.id = LoadProgram("quad");
      quadShader.shadowMap =
        glGetUniformLocation(quadShader.id, "shadowMap");
      quadShader.position = glGetAttribLocation(quadShader.id, "position");

      cout << "{\n";
      cout << "  \"Shadow\": {\n";
      cout << "    \"id\": " << shadowShader.id << ",\n";
      cout << "    \"worldDepthTransform\": " << shadowShader.worldDepthTransform << ",\n";
      cout << "    \"modelTransform\": " << shadowShader.modelTransform << ",\n";
      cout << "    \"position\": " << shadowShader.position << "\n";
      cout << "  },\n  {\n";
      cout << "  \"Flat\": {\n";
      cout << "    \"id\": " << modelShader.id << ",\n";
      cout << "    \"worldTransform\": " << modelShader.worldTransform << ",\n";
      cout << "    \"depthBiasTransform\": " << modelShader.depthBiasTransform << ",\n";
      cout << "    \"shadowMap\": " << modelShader.shadowMap << ",\n";
      cout << "    \"lightDirection\": " << modelShader.lightDirection << ",\n";
      cout << "    \"modelTransform\": " << modelShader.modelTransform << ",\n";
      cout << "    \"position\": " << modelShader.position << ",\n";
      cout << "    \"normal\": " << modelShader.normal << ",\n";
      cout << "    \"color\": " << modelShader.color << "\n";
      cout << "  },\n  {\n";
      cout << "  \"Quad\": {\n";
      cout << "    \"position\": " << quadShader.position << ",\n";
      cout << "    \"shadowMap\": " << quadShader.shadowMap << ",\n";
      cout << "  }\n}\n";

      glGenVertexArrays(1, &quadVertexArrayId);
      glBindVertexArray(quadVertexArrayId);

      glGenBuffers(1, &quadVerticiesId);
      glBindBuffer(GL_ARRAY_BUFFER, quadVerticiesId);
      glBufferData(
        GL_ARRAY_BUFFER, sizeof(quadVerticies), quadVerticies, GL_STATIC_DRAW);

      glGenVertexArrays(1, &vertexArrayId);
      glBindVertexArray(vertexArrayId);

      glGenBuffers(1, &verticiesId);
      glBindBuffer(GL_ARRAY_BUFFER, verticiesId);
      glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

      glGenBuffers(1, &indiciesId);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesId);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indicies), indicies, GL_STATIC_DRAW);

      glGenBuffers(1, &instancesId);
      glBindBuffer(GL_ARRAY_BUFFER, instancesId);
      glBufferData(GL_ARRAY_BUFFER,
        sizeof(instances), instances, GL_STREAM_DRAW);

      glGenTextures(1, &shadowTextureId);
      glBindTexture(GL_TEXTURE_2D, shadowTextureId);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
      glBindTexture(GL_TEXTURE_2D, 0);

      glGenFramebuffers(1, &frameBufferId);
      glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
      glDrawBuffer(GL_NONE);
      glReadBuffer(GL_NONE);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTextureId, 0);

      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Bus::Emit(Procedure::Quit, 0, nullptr);

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    });

    Bus::On(Event::OnPhysicsDelta, +[](long id, double dt) {
      instances[0].modelTransform = rotate(
        instances[0].modelTransform, (float)(dt), vec3(1.0f, 0.0f, 0.0f));
      glBindBuffer(GL_ARRAY_BUFFER, instancesId);
      glBufferData(GL_ARRAY_BUFFER, sizeof(instances), nullptr, GL_STREAM_DRAW);
      glBufferData(GL_ARRAY_BUFFER, sizeof(instances), instances, GL_STREAM_DRAW);
    });

    Bus::On(Event::OnDisplayDelta, +[](long id, double dt) {
      glBindFramebuffer(GL_FRAMEBUFFER, frameBufferId);
      glViewport(0, 0, 1024, 1024);
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glUseProgram(shadowShader.id);
      glBindVertexArray(vertexArrayId);
      glBindBuffer(GL_ARRAY_BUFFER, verticiesId);
      glEnableVertexAttribArray(shadowShader.position);
      glVertexAttribPointer(shadowShader.position, 3, GL_FLOAT, GL_FALSE,
        sizeof(vertex), nullptr);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesId);
      glBindBuffer(GL_ARRAY_BUFFER, instancesId);
      for (auto i = 0; i < 4; i++) {
        glVertexAttribPointer(shadowShader.modelTransform + i,
          4, GL_FLOAT, GL_FALSE,
          sizeof(instance), (void*)(sizeof(vec4) * i));
        glEnableVertexAttribArray(shadowShader.modelTransform + i);
        glVertexAttribDivisor(shadowShader.modelTransform + i, 1);
      }
      glUniformMatrix4fv(shadowShader.worldDepthTransform,
        1, GL_FALSE, value_ptr(worldDepthTransform));
      glDrawElementsInstanced(GL_TRIANGLES, indicies_count, GL_UNSIGNED_INT, nullptr, instances_count);
      glDisableVertexAttribArray(shadowShader.position);
      for (auto i = 0; i < 4; i++)
        glDisableVertexAttribArray(shadowShader.modelTransform + i);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
      glUseProgram(0);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glViewport(0, 0, 1920, 1080);
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      glUseProgram(modelShader.id);
      glBindVertexArray(vertexArrayId);
      glBindBuffer(GL_ARRAY_BUFFER, verticiesId);
      glEnableVertexAttribArray(modelShader.position);
      glVertexAttribPointer(modelShader.position, 3, GL_FLOAT, GL_FALSE,
        sizeof(vertex), nullptr);
      glEnableVertexAttribArray(modelShader.normal);
      glVertexAttribPointer(modelShader.normal, 3,
        GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)sizeof(vec3));
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesId);
      glBindBuffer(GL_ARRAY_BUFFER, instancesId);
      for (auto i = 0; i < 4; i++) {
        glVertexAttribPointer(modelShader.modelTransform + i,
          4, GL_FLOAT, GL_FALSE,
          sizeof(instance), (void*)(sizeof(vec4) * i));
        glEnableVertexAttribArray(modelShader.modelTransform + i);
        glVertexAttribDivisor(modelShader.modelTransform + i, 1);
      }
      glVertexAttribPointer(modelShader.color,
          4, GL_FLOAT, GL_FALSE,
          sizeof(instance), (void*)(offsetof(instance, color)));
      glEnableVertexAttribArray(modelShader.color);
      glVertexAttribDivisor(modelShader.color, 1);
      glUniformMatrix4fv(modelShader.worldTransform, 1, GL_FALSE,
        value_ptr(worldTransform));
      glUniformMatrix4fv(modelShader.depthBiasTransform, 1, GL_FALSE,
        value_ptr(depthBiasTransform));
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, shadowTextureId);
      glUniform1i(modelShader.shadowMap, 0);
      glUniform3fv(modelShader.lightDirection, 1, value_ptr(lightDirection));
      glDrawElementsInstanced(GL_TRIANGLES, indicies_count, GL_UNSIGNED_INT, nullptr, instances_count);
      glDisableVertexAttribArray(modelShader.position);
      glDisableVertexAttribArray(modelShader.normal);
      for (auto i = 0; i < 4; i++)
        glDisableVertexAttribArray(modelShader.modelTransform + i);
      glDisableVertexAttribArray(modelShader.color);
      glBindTexture(GL_TEXTURE_2D, 0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      glBindVertexArray(0);
      glUseProgram(0);

      // glCullFace(GL_NONE);
      // glViewport(0, 0, 1024, 1024);
      // glUseProgram(quadShader.id);
      // glBindVertexArray(quadVertexArrayId);
      // glActiveTexture(GL_TEXTURE0);
      // glBindTexture(GL_TEXTURE_2D, shadowTextureId);
      // glUniform1i(quadShader.shadowMap, 0);
      // glEnableVertexAttribArray(quadShader.position);
      // glBindBuffer(GL_ARRAY_BUFFER, quadVerticiesId);
      // glVertexAttribPointer(quadShader.position, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
      // glDrawArrays(GL_TRIANGLES, 0, 6);
      // glDisableVertexAttribArray(quadShader.position);
      // glBindTexture(GL_TEXTURE_2D, 0);
      // glBindVertexArray(0);
      // glUseProgram(0);
    });

    Bus::On(Event::OnQuit, +[](long id, void* m) {
      glDeleteBuffers(1, &verticiesId);
      glDeleteVertexArrays(1, &vertexArrayId);
      glDeleteBuffers(1, &quadVerticiesId);
      glDeleteVertexArrays(1, &quadVertexArrayId);
      glDeleteBuffers(1, &indiciesId);
      glDeleteBuffers(1, &instancesId);
      glDeleteProgram(modelShader.id);
      glDeleteProgram(shadowShader.id);
      glDeleteProgram(quadShader.id);
      glDeleteFramebuffers(1, &frameBufferId);
      glDeleteTextures(1, &shadowTextureId);
    });
  }
}
