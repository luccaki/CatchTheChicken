#include "window.hpp"

#include <unordered_map>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

float Window::mapValue(float input, float inMin, float inMax, float outMin,
                       float outMax) {
  return outMin + (outMax - outMin) * (input - inMin) / (inMax - inMin);
}

void Window::setRandomRotation() {
  auto &re{m_randomEngine};
  m_chickenRotation = m_randomRotation(re);
}

void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_KEYDOWN) {
    if (m_gameStatus == Start) {
      m_gameStatus = Playing;
      caughtChickens = 0;
      m_chickenPos = {0.0f, 0.14f, 0.0f};
      setRandomRotation();
      m_timer.restart();
    }
    if (event.key.keysym.sym == SDLK_w)
      m_dollySpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_s)
      m_dollySpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_a)
      m_truckSpeed = -1.0f;
    if (event.key.keysym.sym == SDLK_d)
      m_truckSpeed = 1.0f;
    if (event.key.keysym.sym == SDLK_RETURN)
      m_gameStatus = Start;
    if (event.key.keysym.sym == SDLK_SPACE &&
        glm::distance2(m_chickenPos, m_camera.m_eye) <= 0.5 &&
        m_gameStatus == Playing) {
      m_chickenPos = {0.0f, 0.14f, 0.0f};
      setRandomRotation();
      caughtChickens++;
    }
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_w && m_dollySpeed > 0)
      m_dollySpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_s && m_dollySpeed < 0)
      m_dollySpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_a && m_truckSpeed < 0)
      m_truckSpeed = 0.0f;
    if (event.key.keysym.sym == SDLK_d && m_truckSpeed > 0)
      m_truckSpeed = 0.0f;
  }
  if (event.type == SDL_MOUSEMOTION) {
    m_panSpeed = mapValue(event.motion.x, 0, m_viewportSize.x, -1, 1) *
                 m_mouseSensitivity;
  }
}

void Window::onCreate() {
  auto const &assetsPath{abcg::Application::getAssetsPath()};
  auto const seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);
  setRandomRotation();

  auto const filename{assetsPath + "Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::RuntimeError("Cannot load font file");
  }

  abcg::glClearColor(0.5294, 0.8078, 0.9804, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_chickenProgram =
      abcg::createOpenGLProgram({{.source = assetsPath + "shaders/texture.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "shaders/texture.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_ground.create(m_chickenProgram);
  m_wall.create(m_chickenProgram);

  // Get location of uniform variables
  m_viewMatrixLocation =
      abcg::glGetUniformLocation(m_chickenProgram, "viewMatrix");
  m_projMatrixLocation =
      abcg::glGetUniformLocation(m_chickenProgram, "projMatrix");
  m_modelMatrixLocation =
      abcg::glGetUniformLocation(m_chickenProgram, "modelMatrix");
  m_colorLocation = abcg::glGetUniformLocation(m_chickenProgram, "color");

  // Load model
  loadDiffuseTexture(assetsPath + "maps/chicken.png");
  loadObj(assetsPath + "chicken.obj");
  setupVAO(m_chickenProgram);
}

void Window::createBuffers() {
  // Delete previous buffers
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);

  // VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER,
                     sizeof(m_vertices.at(0)) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices.at(0)) * m_indices.size(),
                     m_indices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Window::setupVAO(GLuint program) {
  // Release previous VAO
  abcg::glDeleteVertexArrays(1, &m_VAO);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);
  abcg::glBindVertexArray(m_VAO);

  // Bind EBO and VBO
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  // Bind vertex attributes
  auto const positionAttribute{
      abcg::glGetAttribLocation(program, "inPosition")};
  if (positionAttribute >= 0) {
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex), nullptr);
  }

  auto const normalAttribute{abcg::glGetAttribLocation(program, "inNormal")};
  if (normalAttribute >= 0) {
    abcg::glEnableVertexAttribArray(normalAttribute);
    auto const offset{offsetof(Vertex, normal)};
    abcg::glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex),
                                reinterpret_cast<void *>(offset));
  }

  auto const texCoordAttribute{
      abcg::glGetAttribLocation(program, "inTexCoord")};
  if (texCoordAttribute >= 0) {
    abcg::glEnableVertexAttribArray(texCoordAttribute);
    auto const offset{offsetof(Vertex, texCoord)};
    abcg::glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex),
                                reinterpret_cast<void *>(offset));
  }

  // End of binding
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);
}

void Window::loadDiffuseTexture(std::string_view path) {
  abcg::glDeleteTextures(1, &m_diffuseTexture);
  m_diffuseTexture = abcg::loadOpenGLTexture({.path = path});
}

void Window::computeNormals() {
  // Clear previous vertex normals
  for (auto &vertex : m_vertices) {
    vertex.normal = glm::vec3(0.0f);
  }

  // Compute face normals
  for (std::size_t offset = 0; offset < m_indices.size(); offset += 3) {
    // Get face vertices
    auto &a{m_vertices.at(m_indices.at(offset + 0))};
    auto &b{m_vertices.at(m_indices.at(offset + 1))};
    auto &c{m_vertices.at(m_indices.at(offset + 2))};

    // Compute normal
    auto const edge1{b.position - a.position};
    auto const edge2{c.position - b.position};
    auto const normal{glm::cross(edge1, edge2)};

    // Accumulate on vertices
    a.normal += normal;
    b.normal += normal;
    c.normal += normal;
  }

  // Normalize
  for (auto &vertex : m_vertices) {
    vertex.normal = glm::normalize(vertex.normal);
  }

  m_hasNormals = true;
}

void Window::standardize() {
  // Center to origin and normalize largest bound to [-1, 1]

  // Get bounds
  glm::vec3 max(std::numeric_limits<float>::lowest());
  glm::vec3 min(std::numeric_limits<float>::max());
  for (auto const &vertex : m_vertices) {
    max = glm::max(max, vertex.position);
    min = glm::min(min, vertex.position);
  }

  // Center and scale
  auto const center{(min + max) / 2.0f};
  auto const scaling{2.0f / glm::length(max - min)};
  for (auto &vertex : m_vertices) {
    vertex.position = (vertex.position - center) * scaling;
  }
}

void Window::loadObj(std::string_view path, bool standardize) {
  auto const basePath{abcg::Application::getAssetsPath()};

  tinyobj::ObjReaderConfig readerConfig;
  readerConfig.mtl_search_path = basePath; // Path to material files

  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data(), readerConfig)) {
    if (!reader.Error().empty()) {
      throw abcg::RuntimeError(
          fmt::format("Failed to load model {} ({})", path, reader.Error()));
    }
    throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  auto const &attrib{reader.GetAttrib()};
  auto const &shapes{reader.GetShapes()};
  auto const &materials{reader.GetMaterials()};

  m_vertices.clear();
  m_indices.clear();

  m_hasNormals = false;
  m_hasTexCoords = false;

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (auto const &shape : shapes) {
    // Loop over indices
    for (auto const offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      auto const index{shape.mesh.indices.at(offset)};

      // Position
      auto const startIndex{3 * index.vertex_index};
      glm::vec3 position{attrib.vertices.at(startIndex + 0),
                         attrib.vertices.at(startIndex + 1),
                         attrib.vertices.at(startIndex + 2)};

      // Normal
      glm::vec3 normal{};
      if (index.normal_index >= 0) {
        m_hasNormals = true;
        auto const normalStartIndex{3 * index.normal_index};
        normal = {attrib.normals.at(normalStartIndex + 0),
                  attrib.normals.at(normalStartIndex + 1),
                  attrib.normals.at(normalStartIndex + 2)};
      }

      // Texture coordinates
      glm::vec2 texCoord{};
      if (index.texcoord_index >= 0) {
        m_hasTexCoords = true;
        auto const texCoordsStartIndex{2 * index.texcoord_index};
        texCoord = {attrib.texcoords.at(texCoordsStartIndex + 0),
                    attrib.texcoords.at(texCoordsStartIndex + 1)};
      }

      Vertex const vertex{
          .position = position, .normal = normal, .texCoord = texCoord};

      // If hash doesn't contain this vertex
      if (!hash.contains(vertex)) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }

  // Use properties of first material, if available
  if (!materials.empty()) {
    auto const &mat{materials.at(0)}; // First material
    m_Ka = {mat.ambient[0], mat.ambient[1], mat.ambient[2], 1};
    m_Kd = {mat.diffuse[0], mat.diffuse[1], mat.diffuse[2], 1};
    m_Ks = {mat.specular[0], mat.specular[1], mat.specular[2], 1};
    m_shininess = mat.shininess;

    if (!mat.diffuse_texname.empty())
      loadDiffuseTexture(basePath + mat.diffuse_texname);
  } else {
    // Default values
    m_Ka = {0.1f, 0.1f, 0.1f, 1.0f};
    m_Kd = {0.7f, 0.7f, 0.7f, 1.0f};
    m_Ks = {1.0f, 1.0f, 1.0f, 1.0f};
    m_shininess = 25.0f;
  }

  if (standardize) {
    Window::standardize();
  }

  if (!m_hasNormals) {
    computeNormals();
  }

  createBuffers();
}

void Window::onPaint() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Update chicken pos
  // Calculate the displacement based on the rotation angle
  if (m_gameStatus == Playing) {
    m_chickenPos.x +=
        deltaTime * m_chickenSpeed * glm::sin(glm::radians(m_chickenRotation));
    m_chickenPos.z +=
        deltaTime * m_chickenSpeed * glm::cos(glm::radians(m_chickenRotation));

    if (m_timer.elapsed() > playTime) {
      m_gameStatus = GameOver;
      if (caughtChickens > maxCaughtChickens)
        maxCaughtChickens = caughtChickens;
    }
  } else {
    m_timer.restart();
  }

  if (m_chickenPos.x < -4.8f) {
    m_chickenPos.x = -4.8f;
    setRandomRotation();
  }
  if (m_chickenPos.x > 4.8f) {
    m_chickenPos.x = 4.8f;
    setRandomRotation();
  }
  if (m_chickenPos.z < -4.8f) {
    m_chickenPos.z = -4.8f;
    setRandomRotation();
  }
  if (m_chickenPos.z > 4.8f) {
    m_chickenPos.z = 4.8f;
    setRandomRotation();
  }

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_chickenProgram);

  auto const normalMatrixLoc{
      abcg::glGetUniformLocation(m_chickenProgram, "normalMatrix")};
  auto const lightDirLoc{
      abcg::glGetUniformLocation(m_chickenProgram, "lightDirWorldSpace")};
  auto const shininessLoc{
      abcg::glGetUniformLocation(m_chickenProgram, "shininess")};
  auto const KaLoc{abcg::glGetUniformLocation(m_chickenProgram, "Ka")};
  auto const KdLoc{abcg::glGetUniformLocation(m_chickenProgram, "Kd")};
  auto const KsLoc{abcg::glGetUniformLocation(m_chickenProgram, "Ks")};
  auto const diffuseTexLoc{
      abcg::glGetUniformLocation(m_chickenProgram, "diffuseTex")};
  auto const isChickenLoc{
      abcg::glGetUniformLocation(m_chickenProgram, "isChicken")};
  abcg::glUniform1f(isChickenLoc, true);

  abcg::glUniform1i(diffuseTexLoc, 0);
  abcg::glUniform4fv(lightDirLoc, 1, &m_lightDir.x);

  auto const modelViewMatrix{
      glm::mat3(m_viewMatrixLocation * m_projMatrixLocation)};
  auto const normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  abcg::glUniform4fv(KaLoc, 1, &m_Ka.x);
  abcg::glUniform4fv(KdLoc, 1, &m_Kd.x);
  abcg::glUniform4fv(KsLoc, 1, &m_Ks.x);
  abcg::glUniform1f(shininessLoc, m_shininess);

  // Set uniform variables for viewMatrix and projMatrix
  // These matrices are used for every scene object
  abcg::glUniformMatrix4fv(m_viewMatrixLocation, 1, GL_FALSE,
                           &m_camera.getViewMatrix()[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLocation, 1, GL_FALSE,
                           &m_camera.getProjMatrix()[0][0]);

  abcg::glBindVertexArray(m_VAO);

  // Draw Chicken
  glm::mat4 model{1.0f};
  model = glm::translate(model, m_chickenPos);
  model =
      glm::rotate(model, glm::radians(m_chickenRotation), glm::vec3(0, 1, 0));
  model = glm::scale(model, glm::vec3(0.2f));

  abcg::glUniformMatrix4fv(m_modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
  abcg::glUniform4f(m_colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);

  abcg::glActiveTexture(GL_TEXTURE0);
  abcg::glBindTexture(GL_TEXTURE_2D, m_diffuseTexture);

  // Set minification and magnification parameters
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Set texture wrapping parameters
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT,
                       nullptr);

  abcg::glBindVertexArray(0);

  // Draw ground and Walls

  abcg::glUniform1f(isChickenLoc, false);
  m_ground.paint();
  m_wall.paint();

  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  {
    auto const size{ImVec2(1000, 250)};
    auto const position{ImVec2((m_viewportSize.x - size.x) / 2.0f, 0)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoInputs};
    ImGui::Begin("Timer", nullptr, flags);
    ImGui::PushFont(m_font);

    ImGui::SetCursorPosX(
        (ImGui::GetWindowSize().x -
         ImGui::CalcTextSize(
             fmt::format("{0} chickens", caughtChickens).c_str())
             .x) *
        0.5f);
    ImGui::Text(fmt::format("{0} chickens", caughtChickens).c_str());
    ImGui::SetCursorPosX(
        (ImGui::GetWindowSize().x -
         ImGui::CalcTextSize(
             fmt::format("{0}", static_cast<int>(std::ceil(playTime - m_timer.elapsed())))
                 .c_str())
             .x) *
        0.5f);
    ImGui::Text(
        fmt::format("{0}", static_cast<int>(std::ceil(playTime - m_timer.elapsed())))
            .c_str());

    ImGui::PopFont();
    ImGui::End();
  }

  {
    auto const size{ImVec2(1000, 400)};
    auto const position{ImVec2((m_viewportSize.x - size.x) / 2.0f,
                               (m_viewportSize.y - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoInputs};
    ImGui::Begin("GameStatus", nullptr, flags);
    ImGui::PushFont(m_font);

    switch (m_gameStatus) {
    case GameStatus::Start:
      ImGui::SetCursorPosX(
          (ImGui::GetWindowSize().x -
           ImGui::CalcTextSize("Press any button to start!").x) *
          0.5f);
      ImGui::Text("Press any button to start!");
      break;
    case GameStatus::GameOver:
      ImGui::SetCursorPosX(
          (ImGui::GetWindowSize().x - ImGui::CalcTextSize("Game Over!").x) *
          0.5f);
      ImGui::Text("Game Over!");
      ImGui::SetCursorPosX(
          (ImGui::GetWindowSize().x -
           ImGui::CalcTextSize(
               fmt::format("You caught {0} chickens", caughtChickens).c_str())
               .x) *
          0.5f);
      ImGui::Text(
          fmt::format("You caught {0} chickens", caughtChickens).c_str());
      ImGui::SetCursorPosX((ImGui::GetWindowSize().x -
                            ImGui::CalcTextSize("Press ENTER to restart!").x) *
                           0.5f);
      ImGui::Text("Press ENTER to restart!");
      ImGui::SetCursorPosX(
          (ImGui::GetWindowSize().x -
           ImGui::CalcTextSize(
               fmt::format("Record is {0} chickens", maxCaughtChickens)
                   .c_str())
               .x) *
          0.5f);
      ImGui::Text(
          fmt::format("Record is {0} chickens", maxCaughtChickens).c_str());
      break;
    default:
      break;
    }

    ImGui::PopFont();
    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_camera.computeProjectionMatrix(size);
}

void Window::onDestroy() {
  m_ground.destroy();
  m_wall.destroy();

  abcg::glDeleteProgram(m_chickenProgram);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Update LookAt camera
  m_camera.dolly(m_dollySpeed * deltaTime);
  m_camera.truck(m_truckSpeed * deltaTime);
  m_camera.pan(m_panSpeed * deltaTime);
}