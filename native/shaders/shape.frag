#version 330 core
out vec4 FragColor;

in vec2 LocalPos;       // Coordenadas locais interpoladas de -0.5 a 0.5
in vec3 Color;
in vec3 BorderColor;
in float ShapeType;     // 0.0 = Retângulo, 1.0 = Círculo, 2.0 = Retângulo de Fundo (sem borda)
in float Dashed;

void main() {
    // Retângulo de Fundo simples (sem bordas)
    if (ShapeType > 1.5) {
        FragColor = vec4(Color, 1.0);
        return;
    }

    // Círculo Analítico (Aminoácidos)
    if (ShapeType > 0.5) {
        float dist = length(LocalPos);
        if (dist > 0.5) {
            discard; // Recorta fragmentos fora do círculo
        }

        // Determinação de Borda vs Preenchimento
        if (dist > 0.44) {
            if (Dashed > 0.5) {
                float angle = atan(LocalPos.y, LocalPos.x);
                // Divide o círculo em 8 segmentos para o tracejado
                if (mod(floor(angle * 4.0), 2.0) == 0.0) {
                    FragColor = vec4(Color, 1.0);
                    return;
                }
            }
            FragColor = vec4(BorderColor, 1.0);
        } else {
            FragColor = vec4(Color, 1.0);
        }
    }
    // Retângulo (Nucleotídeos)
    else {
        bool isBorder = (abs(LocalPos.x) > 0.44 || abs(LocalPos.y) > 0.44);

        if (isBorder) {
            if (Dashed > 0.5) {
                float pattern = (abs(LocalPos.y) > 0.44) ? LocalPos.x : LocalPos.y;
                // Aplica 16 divisões na borda para gerar o tracejado
                if (mod(floor(pattern * 16.0), 2.0) == 0.0) {
                    FragColor = vec4(Color, 1.0);
                    return;
                }
            }
            FragColor = vec4(BorderColor, 1.0);
        } else {
            FragColor = vec4(Color, 1.0);
        }
    }
}
