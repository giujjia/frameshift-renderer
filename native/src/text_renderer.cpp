#include <glad/glad.h>
#include "text_renderer.h"

std::map<char, Character> Characters;
unsigned int textVAO = 0;
unsigned int textVBO = 0;
unsigned int textShader = 0;
int textProjLoc = -1;
int textViewLoc = -1;
int textUseViewLoc = -1;
int textColorLoc = -1;

float obterLarguraTexto(const std::string& texto, float escala) {
	float largura = 0.0f;
	for (char c : texto) {
		largura += (Characters[c].Advance >> 6) * escala;
	}
	return largura;
}

void RenderText(std::string text, float x, float y, float scale, Vec3 color, bool useViewMatrix) {
	glUseProgram(textShader);
	glUniform3f(textColorLoc, color.r, color.g, color.b);
	glUniform1f(textUseViewLoc, useViewMatrix ? 1.0f : 0.0f);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);

	for (std::string::const_iterator c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y + (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		float vertices[6][4] = {
			{ xpos,     ypos - h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos - h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos - h,   1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.Advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
