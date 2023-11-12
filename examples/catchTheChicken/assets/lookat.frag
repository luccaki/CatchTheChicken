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
