#ifndef WALL_HPP_
#define WALL_HPP_

#include "abcgOpenGL.hpp"

class Wall {
public:
  void create(GLuint program);
  void paint();
  void destroy();

private:
  GLuint m_VAOWall{};
  GLuint m_VBOWall{};

  GLint m_modelMatrixLoc{};
  GLint m_colorLoc{};

  std::array<glm::vec3, 4> verticesWall{{{-5.0f, 0.0f, +5.0f},
                                              {-5.0f, 1.0f, +5.0f},
                                              {+5.0f, 0.0f, +5.0f},
                                              {+5.0f, 1.0f, +5.0f}}};

  glm::vec4 wallColor{0.251f, 0.184f, 0.039f, 1.0f};
};

#endif