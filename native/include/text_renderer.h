#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include "graphics_math.h"
#include <string>
#include <map>

struct Character {
	unsigned int TextureID;
	Vec2 Size;
	Vec2 Bearing;
	unsigned int Advance;
};

extern std::map<char, Character> Characters;
extern unsigned int textVAO, textVBO;
extern unsigned int textShader;
extern int textProjLoc, textViewLoc, textUseViewLoc, textColorLoc;

float obterLarguraTexto(const std::string& texto, float escala);
void RenderText(std::string text, float x, float y, float scale, Vec3 color, bool useViewMatrix);

#endif
