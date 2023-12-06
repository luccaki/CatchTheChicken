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