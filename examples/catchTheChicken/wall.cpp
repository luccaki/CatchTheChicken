#include "wall.hpp"

void Wall::create(GLuint program) {
  auto const positionAttribute{
      abcg::glGetAttribLocation(program, "inPosition")};

  // Front Wall
  abcg::glGenBuffers(1, &m_VBOFrontWall);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOFrontWall);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(verticesFrontWall),
                     verticesFrontWall.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glGenVertexArrays(1, &m_VAOFrontWall);
  abcg::glBindVertexArray(m_VAOFrontWall);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOFrontWall);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);

  // Back Wall
  abcg::glGenBuffers(1, &m_VBOBackWall);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOBackWall);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBackWall),
                     verticesBackWall.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glGenVertexArrays(1, &m_VAOBackWall);
  abcg::glBindVertexArray(m_VAOBackWall);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOBackWall);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);

  // Left Wall
  abcg::glGenBuffers(1, &m_VBOLeftWall);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOLeftWall);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLeftWall),
                     verticesLeftWall.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glGenVertexArrays(1, &m_VAOLeftWall);
  abcg::glBindVertexArray(m_VAOLeftWall);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOLeftWall);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);

  // Right Wall
  abcg::glGenBuffers(1, &m_VBORightWall);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBORightWall);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRightWall),
                     verticesRightWall.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glGenVertexArrays(1, &m_VAORightWall);
  abcg::glBindVertexArray(m_VAORightWall);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBORightWall);
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
  model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(1.0f));

  abcg::glBindVertexArray(m_VAOFrontWall);
  abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLoc, wallColor[0], wallColor[1], wallColor[2], wallColor[3]);
  abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  abcg::glBindVertexArray(0);

  abcg::glBindVertexArray(m_VAOBackWall);
  abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLoc, wallColor[0], wallColor[1], wallColor[2], wallColor[3]);
  abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  abcg::glBindVertexArray(0);

  abcg::glBindVertexArray(m_VAOLeftWall);
  abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLoc, wallColor[0], wallColor[1], wallColor[2], wallColor[3]);
  abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  abcg::glBindVertexArray(0);

  abcg::glBindVertexArray(m_VAORightWall);
  abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLoc, wallColor[0], wallColor[1], wallColor[2], wallColor[3]);
  abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  abcg::glBindVertexArray(0);
}

void Wall::destroy() {
  abcg::glDeleteBuffers(1, &m_VBOFrontWall);
  abcg::glDeleteBuffers(1, &m_VBOBackWall);
  abcg::glDeleteBuffers(1, &m_VBOLeftWall);
  abcg::glDeleteBuffers(1, &m_VBORightWall);
  abcg::glDeleteVertexArrays(1, &m_VAOFrontWall);
  abcg::glDeleteVertexArrays(1, &m_VAOBackWall);
  abcg::glDeleteVertexArrays(1, &m_VAOLeftWall);
  abcg::glDeleteVertexArrays(1, &m_VAORightWall);
}