Lucca Ianaguivara Kisanucki - RA: 11201812090

# Catch the Chicken

Link para a aplicação: https://luccaki.github.io/CatchTheChicken/

Um jogo 3D, onde o objetivo é conseguir pegar o máximo de galinhas possiveis.

## Movimentação da camera / Visão

Código baseado no projeto lookat, porém foi feito modificações na movimentação da camera, foi adicionado o controle por mouse da movimentação lateral da tela.

No método onEvent():
```
  if (event.type == SDL_MOUSEMOTION) {
    m_panSpeed = mapValue(event.motion.x, 0, m_viewportSize.x, -1, 1) * m_mouseSensitivity;
  }
```
Dependendo da posição do mouse no eixo X, ele faz um mapa para um valor entre -1 e 1, sendo a velocidade que ele vai virar a tela, quando negativo vira para esquerda, quando positivo vira para direita, a tela só fica parada quando o valor é zero e o mouse está no centro da tela.

m_panSpeed representa a movimentação da camera no eixo x.

event.motion.x pega o movimento do mouse no eixo.

fazendo o map dos valores conseguimos transformar a movimentação do mouse para a movimentação da camera.

## Parede / Cerca

Além do chão do projeto lookat foi adicionado 4 paredes, que funcionam como uma cerca, para limitar a movimentação da galinha e do jogador.

A parede contém os seguintes vertices:
```
  std::array<glm::vec3, 4> verticesWall{{{-5.0f, 0.0f, +5.0f},
                                              {-5.0f, 1.0f, +5.0f},
                                              {+5.0f, 0.0f, +5.0f},
                                              {+5.0f, 1.0f, +5.0f}}};
```
Cada uma é criada fazendo a rotação em 0, 90, 180 e 270º
```
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
```

Além disso no código da camera, foi adicionado uma verificação da posição, para não ultrapassar a cerca:
```
void Camera::computeViewMatrix() {
  if (m_eye.x < -4.8f)
    m_eye.x = -4.8f;
  if (m_eye.x > 4.8f)
    m_eye.x = 4.8f;
  if (m_eye.z < -4.8f)
    m_eye.z = -4.8f;
  if (m_eye.z > 4.8f)
    m_eye.z = 4.8f;

  m_viewMatrix = glm::lookAt(m_eye, m_at, m_up);
}
```
Um quadrado imaginário entre as coordenadas -4.8 e +4.8, pois as paredes vai de -5 a +5

m_eye.x representa a coordenada x da posição da camera;

m_eye.z representa a coordenada z da posição da camera;

## Galinha

A galinha é gerada usando o arquivo chicken.obj e chicken.png que contém os vertices e a texture dela.

Usando os comandos vistos em aula:
```
loadDiffuseTexture(assetsPath + "maps/chicken.png");
loadObj(assetsPath + "chicken.obj");
```

Foi adicionado tambem uma limitação do movimento da galinha num quadra 4.8 x 4.8.
```
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
```
onde, m_chickenPos é a posição da galinha em x, y e z

E a cada vez que ele bate na parede ele escolhe um angulo aleatorio para ser o proximo movimento, com o metodo setRandomRotation():
```
void Window::setRandomRotation() {
  auto &re{m_randomEngine};
  m_chickenRotation = m_randomRotation(re);
}
```
onde, m_chickenRotation é o angulo entre 0 e 360.

A movimentação da galinha é feita mundando a posição da galinha em relação a seu angulo de rotação
```
m_chickenPos.x += deltaTime * m_chickenSpeed * glm::sin(glm::radians(m_chickenRotation));
m_chickenPos.z += deltaTime * m_chickenSpeed * glm::cos(glm::radians(m_chickenRotation));
```
a cada tick, é realizado o incremento nas posições x e z, onde x recebe o seno do angulo, e o z o cosseno do angulo.

conseguindo assim realizar movimentos nas diagonais, dependendo do angulo.

## Vertex e Fragment Shader

Para o vertex shader foi implementado uma variação do shader do viewer4 e lookat, para trabalhar com a vizualização de camera e iluminação da textura.
```
#version 300 es

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

uniform vec4 color;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat3 normalMatrix;

uniform vec4 lightDirWorldSpace;

out vec3 fragV;
out vec3 fragL;
out vec3 fragN;
out vec2 fragTexCoord;
out vec3 fragPObj;
out vec3 fragNObj;
out vec4 fragColor;

void main() {
  vec3 P = (viewMatrix * modelMatrix * vec4(inPosition, 1.0)).xyz;
  vec3 N = normalMatrix * inNormal;
  vec3 L = -(viewMatrix * lightDirWorldSpace).xyz;

  fragL = L;
  fragV = -P;
  fragN = N;
  fragTexCoord = inTexCoord;
  fragPObj = inPosition;
  fragNObj = inNormal;
  fragColor = color;

  vec4 posEyeSpace = viewMatrix * modelMatrix * vec4(inPosition, 1);
  gl_Position = projMatrix * posEyeSpace;
}
```

Para o fragment shader, foi implementado duas funcionalidades, dependendo da variavel isChicken:

se isChicken = true, ele retorna a função Blinn-Phong com textura do viewer4;

se isChicken = false, ele usa uma função aleatorio que adiciona ruido na cor, dando um aspecto de TVs antigas;
```
#version 300 es

precision mediump float;

in vec4 fragColor;
in vec3 fragN;
in vec3 fragL;
in vec3 fragV;
in vec2 fragTexCoord;
in vec3 fragPObj;
in vec3 fragNObj;

// Material properties
uniform vec4 Ka, Kd, Ks;
uniform float shininess;
uniform bool isChicken;

// Diffuse texture sampler
uniform sampler2D diffuseTex;

out vec4 outColor;

// Blinn-Phong reflection model
vec4 BlinnPhong(vec3 N, vec3 L, vec3 V, vec2 texCoord) {
  N = normalize(N);
  L = normalize(L);

  // Compute lambertian term
  float lambertian = max(dot(N, L), 0.0);

  // Compute specular term
  float specular = 0.0;
  if (lambertian > 0.0) {
    V = normalize(V);
    vec3 H = normalize(L + V);
    float angle = max(dot(H, N), 0.0);
    specular = pow(angle, shininess);
  }

  vec4 map_Kd = texture(diffuseTex, texCoord);
  vec4 map_Ka = map_Kd;

  vec4 diffuseColor = map_Kd * Kd * lambertian;
  vec4 specularColor = Ks * specular;
  vec4 ambientColor = map_Ka * Ka;

  return ambientColor + diffuseColor + specularColor;
}

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    if(isChicken){
      vec4 color;
      vec2 texCoord;
      texCoord = fragTexCoord;
      color = BlinnPhong(fragN, fragL, fragV, texCoord);
      outColor = color;
    }
    else{
      float randomValue1 = rand(gl_FragCoord.xy);
      float randomValue2 = rand(gl_FragCoord.yx);
      vec3 randomOffset = vec3(randomValue1, randomValue2, rand(vec2(randomValue1, randomValue2)));
      randomOffset = randomOffset * 0.2 - 0.1;
  
      outColor = vec4(clamp(fragColor.rgb + randomOffset, 0.0, 1.0), fragColor.a);
    }
}
```

No metodo OnPaint(), é inicializado isChicken como 'true' para desenhar a galinha, e depois para desenhar a parade e o chão é setado como 'false'
```
  auto const isChickenLoc{abcg::glGetUniformLocation(m_chickenProgram, "isChicken")};
  abcg::glUniform1f(isChickenLoc, true);

  .
  // Draw chicken
  .

  abcg::glUniform1f(isChickenLoc, false);
  m_ground.paint();
  m_wall.paint();
```
Ficando com essa aparência:

![image](https://github.com/luccaki/CatchTheChicken/assets/49926738/8b143f82-c945-4989-bad2-a7e3f22a6480)
