#include "wall.hpp"

void Wall::create(GLuint program) {
  auto const positionAttribute{
      abcg::glGetAttribLocation(program, "inPosition")};

  abcg::glGenBuffers(1, &m_VBOWall);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOWall);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(verticesWall), verticesWall.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glGenVertexArrays(1, &m_VAOWall);
  abcg::glBindVertexArray(m_VAOWall);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOWall);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);

  // Save location of uniform variables
  m_modelMatrixLoc = abcg::glGetUniformLocation(program, "modelMatrix");
  m_colorLoc = abcg::glGetUniformLocation(program, "color");
}

void Wall::paint() {
  glm::mat4 model{1.0f};
  model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
  model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(1.0f));

  abcg::glBindVertexArray(m_VAOWall);
  abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLoc, wallColor[0], wallColor[1], wallColor[2],
                    wallColor[3]);
  abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
  abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLoc, wallColor[0], wallColor[1], wallColor[2],
                    wallColor[3]);
  abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0, 1, 0));
  abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLoc, wallColor[0], wallColor[1], wallColor[2],
                    wallColor[3]);
  abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  model = glm::rotate(model, glm::radians(270.0f), glm::vec3(0, 1, 0));
  abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLoc, wallColor[0], wallColor[1], wallColor[2],
                    wallColor[3]);
  abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  abcg::glBindVertexArray(0);
}

void Wall::destroy() {
  abcg::glDeleteBuffers(1, &m_VBOWall);
  abcg::glDeleteVertexArrays(1, &m_VAOWall);
}