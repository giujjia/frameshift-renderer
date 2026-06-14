#version 330 core
layout (location = 0) in vec2 aPos;         // Vértice local no quad [-0.5, 0.5]

// Atributos por instância (Instance Arrays)
layout (location = 1) in vec2 aOffset;      // Posição na tela (X, Y)
layout (location = 2) in vec2 aScale;       // Escala do elemento (largura, altura)
layout (location = 3) in vec3 aColor;       // Cor de preenchimento (RGB)
layout (location = 4) in vec3 aBorderColor; // Cor da borda (RGB)
layout (location = 5) in float aShapeType;  // Tipo: 0.0 = Retângulo, 1.0 = Círculo, 2.0 = Retângulo de Fundo (sem borda)
layout (location = 6) in float aDashed;     // Flag de borda tracejada (1.0 = ativa)

out vec2 LocalPos;
out vec3 Color;
out vec3 BorderColor;
out float ShapeType;
out float Dashed;

uniform mat4 projection;
uniform mat4 view;

void main() {
    LocalPos = aPos;
    Color = aColor;
    BorderColor = aBorderColor;
    ShapeType = aShapeType;
    Dashed = aDashed;

    // Coordenadas mundiais baseadas nas transformações da instância
    vec2 worldPos = aOffset + aPos * aScale;
    gl_Position = projection * view * vec4(worldPos, 0.0, 1.0);
}
