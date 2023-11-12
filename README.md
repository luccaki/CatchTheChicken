Lucca Ianaguivara Kisanucki - RA: 11201812090

# Catch the Chicken

Link para a aplicação: https://luccaki.github.io/CatchTheChicken/

Um jogo 3D, onde o objetivo é conseguir pegar a galinha.

## Movimentação da camera / Visão

Código baseado no projeto lookat, porém foi feito modificações na movimentação da camera, foi adicionado o controle por mouse da movimentação lateral da tela.

No método onEvent():
```
  if (event.type == SDL_MOUSEMOTION) {
    m_panSpeed = mapValue(event.motion.x, 0, m_viewportSize.x, -1, 1) * m_mouseSensitivity;
  }
```
Dependendo da posição do mouse no eixo X, ele faz um mapa para um valor entre -1 e 1, sendo a velocidade que ele vai virar a tela, quando negativo vira para esquerda, quando positivo vira para direita, a tela só fica parada quando o valor é zero e o mouse está no centro da tela.

Também para melhor a visão, o ponto mais longe da camera foi aumentado para 20.0f:
```
void Camera::computeProjectionMatrix(glm::vec2 const &size) {
  m_projMatrix = glm::mat4(1.0f);
  auto const aspect{size.x / size.y};
  m_projMatrix = glm::perspective(glm::radians(70.0f), aspect, 0.1f, 20.0f);
}
```

## Parede / Cerca

Além do chão do projeto lookat foi adicionado 4 paredes, que funcionam como uma cerca, para limitar a movimentação da galinha e do jogador.

As paredes contém os seguintes vertices:
```
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
```
Cada uma é criada com seu próprio VBO e VAO.

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

## Galinha

A galinha é gerada usando o arquivo chicken.obj que contém os vertices dela.

E para essa primeira versão ela fica apenas girando, então foi adicionado um código para ir aumentando o angulo de rotação do modelo:
```
auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
m_chickenRotation += 10 * deltaTime;
if (m_chickenRotation >= 360)
    m_chickenRotation = 0;

...

model = glm::rotate(model, glm::radians(m_chickenRotation), glm::vec3(0, 1, 0));
```

Para a ultima versão será adicionado a lógica de andar aleatoriamente dentro da cerca.

## Vertex e Fragment Shader

Para o vertex shader foi apenas aumentado o raio de visão:
```
#version 300 es

layout(location = 0) in vec3 inPosition;

uniform vec4 color;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec4 fragColor;

void main() {
  vec4 posEyeSpace = viewMatrix * modelMatrix * vec4(inPosition, 1);

  float i = 1.0 - (-posEyeSpace.z / 7.0);
  fragColor = vec4(i, i, i, 1) * color;

  gl_Position = projMatrix * posEyeSpace;
}
```
Para o fragment shader, foi implementado uma função para gerar tons de cores aleatorias em volta da cor principal, o outColor pode ter valores entre -0.1 e +0.1 em relação ao fragColor:
```
#version 300 es
precision mediump float;

in vec4 fragColor;
out vec4 outColor;

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    float randomValue1 = rand(gl_FragCoord.xy);
    float randomValue2 = rand(gl_FragCoord.yx);
    vec3 randomOffset = vec3(randomValue1, randomValue2, rand(vec2(randomValue1, randomValue2)));
    randomOffset = randomOffset * 0.2 - 0.1;

    outColor = vec4(clamp(fragColor.rgb + randomOffset, 0.0, 1.0), fragColor.a);
}

```
Ficando com essa aparência:
![image](https://github.com/luccaki/CatchTheChicken/assets/49926738/475bd9d9-4b23-4268-a12f-6abbe18bdce1)
