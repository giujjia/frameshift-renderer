#ifndef GRAPHICS_MATH_H
#define GRAPHICS_MATH_H

#pragma pack(push, 1)
struct Vec2 {
	float x, y;
	Vec2() : x(0.0f), y(0.0f) {}
	Vec2(float x, float y) : x(x), y(y) {}
};

struct Vec3 {
	float r, g, b;
	Vec3() : r(0.0f), g(0.0f), b(0.0f) {}
	Vec3(float r, float g, float b) : r(r), g(g), b(b) {}
};

struct InstanceData {
	Vec2 offset;
	Vec2 scale;
	Vec3 color;
	Vec3 borderColor;
	float shapeType;
	float dashed;
};
#pragma pack(pop)

inline void setOrthoMatrix(float left, float right, float bottom, float top, float nearVal, float farVal, float* m) {
	for (int i = 0; i < 16; ++i) m[i] = 0.0f;
	m[0] = 2.0f / (right - left);
	m[5] = 2.0f / (top - bottom);
	m[10] = -2.0f / (farVal - nearVal);
	m[12] = -(right + left) / (right - left);
	m[13] = -(top + bottom) / (top - bottom);
	m[14] = -(farVal + nearVal) / (farVal - nearVal);
	m[15] = 1.0f;
}

inline void setTranslationMatrix(float tx, float ty, float* m) {
	for (int i = 0; i < 16; ++i) m[i] = 0.0f;
	m[0] = 1.0f;
	m[5] = 1.0f;
	m[10] = 1.0f;
	m[12] = tx;
	m[13] = ty;
	m[15] = 1.0f;
}

#endif
