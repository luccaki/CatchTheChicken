#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"

#include "camera.hpp"
#include "ground.hpp"
#include "wall.hpp"

#include <cmath>
#include <random>

struct Vertex {
  glm::vec3 position{};
  glm::vec3 normal{};
  glm::vec2 texCoord{};

  friend bool operator==(Vertex const &, Vertex const &) = default;
};

enum GameStatus {
    Start,
    Playing,
    GameOver
};

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;
  void onUpdate() override;

private:
  glm::ivec2 m_viewportSize{};

  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_chickenProgram{};

  glm::vec4 m_Ka{};
  glm::vec4 m_Kd{};
  glm::vec4 m_Ks{};
  float m_shininess{};
  GLuint m_diffuseTexture{};
  glm::vec4 m_lightDir{10.0f, 0.0f, 0.0f, 0.0f};

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;

  bool m_hasNormals{false};
  bool m_hasTexCoords{false};

  GLint m_viewMatrixLocation{};
  GLint m_projMatrixLocation{};
  GLint m_modelMatrixLocation{};
  GLint m_colorLocation{};

  std::default_random_engine m_randomEngine;
  std::uniform_real_distribution<float> m_randomRotation{0.0f, 360.0f};

  Camera m_camera;
  float m_dollySpeed{};
  float m_truckSpeed{};
  float m_panSpeed{};

  float m_mouseSensitivity{2.0f};

  Ground m_ground;
  Wall m_wall;

  glm::vec3 m_chickenPos{0.0f, 0.14f, 0.0f};
  float m_chickenRotation{0.0f};
  float m_chickenSpeed{0.5f};

  ImFont *m_font{};
  GameStatus m_gameStatus{Start};
  abcg::Timer m_timer;
  int caughtChickens{0};
  int maxCaughtChickens{0};
  double playTime{10};

  //void loadModelFromFile(std::string_view path);
  float mapValue(float input, float inMin, float inMax, float outMin, float outMax);
  void setRandomRotation();
  void loadDiffuseTexture(std::string_view path);
  void loadObj(std::string_view path, bool standardize = true);
  void computeNormals();
  void standardize();
  void setupVAO(GLuint program);
  void createBuffers();
};

#endif