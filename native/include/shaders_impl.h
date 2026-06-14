#ifndef SHADERS_IMPL_H
#define SHADERS_IMPL_H

const char* shapeVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aOffset;
layout (location = 2) in vec2 aScale;
layout (location = 3) in vec3 aColor;
layout (location = 4) in vec3 aBorderColor;
layout (location = 5) in float aShapeType;
layout (location = 6) in float aDashed;

out vec2 LocalPos;
out vec3 Color;
out vec3 BorderColor;
out float ShapeType;
out float Dashed;
out vec2 Scale;

uniform mat4 projection;
uniform mat4 view;

void main() {
    LocalPos = aPos;
    Color = aColor;
    BorderColor = aBorderColor;
    ShapeType = aShapeType;
    Dashed = aDashed;
    Scale = aScale;

    vec2 worldPos = aOffset + aPos * aScale;
    gl_Position = projection * view * vec4(worldPos, 0.0, 1.0);
}
)";

const char* shapeFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 LocalPos;
in vec3 Color;
in vec3 BorderColor;
in float ShapeType;
in float Dashed;
in vec2 Scale;

float roundedBoxSDF(vec2 p, vec2 b, vec4 r) {
    r.xy = (p.x > 0.0) ? r.xy : r.zw;
    r.x  = (p.y > 0.0) ? r.x  : r.y;
    vec2 d = abs(p) - b + r.x;
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - r.x;
}

void main() {
    if (ShapeType > 1.5) {
        FragColor = vec4(Color, 1.0);
        return;
    }

    if (ShapeType > 0.5) {
        float dist = length(LocalPos);
        if (dist > 0.5) {
            discard;
        }
        if (dist > 0.47) {
            if (Dashed > 0.5) {
                float angle = atan(LocalPos.y, LocalPos.x);
                if (mod(floor(angle * 4.0), 2.0) == 0.0) {
                    FragColor = vec4(Color, 1.0);
                    return;
                }
            }
            FragColor = vec4(BorderColor, 1.0);
        } else {
            FragColor = vec4(Color, 1.0);
        }
    } else {
        vec2 p = LocalPos * Scale;
        vec2 b = Scale / 2.0;
        
        if (ShapeType < 0.5) {
            if (Scale.x < 5.0) {
                if (Dashed > 0.5) {
                    float pattern = LocalPos.y;
                    if (mod(floor(pattern * 16.0), 2.0) == 0.0) {
                        discard;
                    }
                }
                FragColor = vec4(Color, 1.0);
                return;
            }
            
            if (Color != BorderColor) {
                bool isBorder = (abs(LocalPos.x) > 0.45 || abs(LocalPos.y) > 0.45);
                if (isBorder) {
                    if (Dashed > 0.5) {
                        float pattern = (abs(LocalPos.y) > 0.45) ? LocalPos.x : LocalPos.y;
                        if (mod(floor(pattern * 16.0), 2.0) == 0.0) {
                            FragColor = vec4(Color, 1.0);
                            return;
                        }
                    }
                    FragColor = vec4(BorderColor, 1.0);
                } else {
                    FragColor = vec4(Color, 1.0);
                }
            } else {
                FragColor = vec4(Color, 1.0);
            }
            return;
        }
        
        float radVal = 8.0;
        vec4 r;
        if (ShapeType < 4.5) {
            r = vec4(radVal, 0.0, radVal, 0.0);
        } else {
            r = vec4(radVal, radVal, radVal, radVal);
        }
        
        float d = roundedBoxSDF(p, b, r);
        if (d > 0.0) {
            discard;
        }
        FragColor = vec4(Color, 1.0);
    }
}
)";

const char* textVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec4 vertex;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform float useView;

void main() {
    if (useView > 0.5) {
        gl_Position = projection * view * vec4(vertex.xy, 0.0, 1.0);
    } else {
        gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    }
    TexCoords = vertex.zw;
}
)";

const char* textFragmentShaderSource = R"(
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main() {    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}
)";

#endif
