#ifndef WALL_HPP_
#define WALL_HPP_

#include "abcgOpenGL.hpp"

class Wall {
public:
  void create(GLuint program);
  void paint();
  void destroy();

private:
  GLuint m_VAOFrontWall{};
  GLuint m_VBOFrontWall{};

  GLuint m_VAOBackWall{};
  GLuint m_VBOBackWall{};

  GLuint m_VAOLeftWall{};
  GLuint m_VBOLeftWall{};

  GLuint m_VAORightWall{};
  GLuint m_VBORightWall{};

  GLint m_modelMatrixLoc{};
  GLint m_colorLoc{};

  std::array<glm::vec3, 4> verticesFrontWall{{{-5.0f, 0.0f, +5.0f},
                                              {-5.0f, 1.0f, +5.0f},
                                              {+5.0f, 0.0f, +5.0f},
                                              {+5.0f, 1.0f, +5.0f}}};

  std::array<glm::vec3, 4> verticesBackWall{{{-5.0f, 0.0f, -5.0f},
                                             {-5.0f, 1.0f, -5.0f},
                                             {+5.0f, 0.0f, -5.0f},
                                             {+5.0f, 1.0f, -5.0f}}};

  std::array<glm::vec3, 4> verticesLeftWall{{{-5.0f, 0.0f, -5.0f},
                                             {-5.0f, 1.0f, -5.0f},
                                             {-5.0f, 0.0f, +5.0f},
                                             {-5.0f, 1.0f, +5.0f}}};

  std::array<glm::vec3, 4> verticesRightWall{{{+5.0f, 0.0f, -5.0f},
                                              {+5.0f, 1.0f, -5.0f},
                                              {+5.0f, 0.0f, +5.0f},
                                              {+5.0f, 1.0f, +5.0f}}};

  glm::vec4 wallColor{0.251f, 0.184f, 0.039f, 1.0f};
};

#endif