#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <algorithm>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "engine.h"
#include "graphics_math.h"
#include "shaders_impl.h"
#include "text_renderer.h"

const float NUC_W = 22.0f;
const float NUC_H = 26.0f;
const float NUC_GAP = 1.5f;
const float CODON_GAP = 14.0f;
const float CODON_W = NUC_W * 3.0f + NUC_GAP * 2.0f;
const float ADVANCE_CODON = CODON_W + CODON_GAP;
const float BALL_R = 19.0f;
const float BALL_D = BALL_R * 2.0f;
const float AA_POS_Y_REF = 84.0f;
const float FITA_Y_REF = AA_POS_Y_REF + BALL_R + 15.0f;
const float AA_POS_Y_MUT = 270.0f;
const float FITA_Y_MUT = AA_POS_Y_MUT + BALL_R + 15.0f;

RenderData g_RenderData;
std::mutex g_RenderMutex;

GLFWwindow* g_Window = nullptr;
unsigned int shapeVAO = 0, shapeVBO = 0, instanceVBO = 0;

unsigned int shapeShader = 0;

int shapeProjLoc = -1, shapeViewLoc = -1;

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 360;

// Controla entrada de teclado para mover a camera
void teclado_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        float velocidade = 32.0f;
        if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D) {
            std::lock_guard<std::mutex> lock(g_RenderMutex);
            g_RenderData.cameraOffsetX += velocidade;
        } else if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A) {
            std::lock_guard<std::mutex> lock(g_RenderMutex);
            g_RenderData.cameraOffsetX = std::max(0.0f, g_RenderData.cameraOffsetX - velocidade);
        }
    }
}

void initEngine() {
    if (!glfwInit()) {
        std::cerr << "Falha ao inicializar o GLFW" << std::endl;
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    g_Window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ShiftGL - Visualizador de Frameshift", NULL, NULL);
    if (g_Window == nullptr) {
        std::cerr << "Falha ao criar a janela do GLFW" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(g_Window);
    glfwSetKeyCallback(g_Window, teclado_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Falha ao inicializar o GLAD" << std::endl;
        return;
    }

    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &shapeVertexShaderSource, NULL);
    glCompileShader(vs);

    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &shapeFragmentShaderSource, NULL);
    glCompileShader(fs);

    shapeShader = glCreateProgram();
    glAttachShader(shapeShader, vs);
    glAttachShader(shapeShader, fs);
    glLinkProgram(shapeShader);

    glDeleteShader(vs);
    glDeleteShader(fs);

    shapeProjLoc = glGetUniformLocation(shapeShader, "projection");
    shapeViewLoc = glGetUniformLocation(shapeShader, "view");

    vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &textVertexShaderSource, NULL);
    glCompileShader(vs);

    fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &textFragmentShaderSource, NULL);
    glCompileShader(fs);

    textShader = glCreateProgram();
    glAttachShader(textShader, vs);
    glAttachShader(textShader, fs);
    glLinkProgram(textShader);

    glDeleteShader(vs);
    glDeleteShader(fs);

    textProjLoc = glGetUniformLocation(textShader, "projection");
    textViewLoc = glGetUniformLocation(textShader, "view");
    textUseViewLoc = glGetUniformLocation(textShader, "useView");
    textColorLoc = glGetUniformLocation(textShader, "textColor");

    float quadVertices[] = {
        -0.5f,  0.5f,
        -0.5f, -0.5f,
         0.5f, -0.5f,

        -0.5f,  0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f
    };

    glGenVertexArrays(1, &shapeVAO);
    glGenBuffers(1, &shapeVBO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(shapeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, shapeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData) * 50000, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(1); // offset
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, offset));
    glVertexAttribDivisor(1, 1);

    glEnableVertexAttribArray(2); // scale
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, scale));
    glVertexAttribDivisor(2, 1);

    glEnableVertexAttribArray(3); // color
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, color));
    glVertexAttribDivisor(3, 1);

    glEnableVertexAttribArray(4); // borderColor
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, borderColor));
    glVertexAttribDivisor(4, 1);

    glEnableVertexAttribArray(5); // shapeType
    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, shapeType));
    glVertexAttribDivisor(5, 1);

    glEnableVertexAttribArray(6); // dashed
    glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)offsetof(InstanceData, dashed));
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);

    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "Erro ao inicializar a biblioteca FreeType" << std::endl;
        return;
    }

    std::string fontPath = "native/resources/fonts/Antonio-Bold.ttf";
    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        fontPath = "../native/resources/fonts/Antonio-Bold.ttf";
        if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
            fontPath = "data/Antonio-Bold.ttf";
            if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
                fontPath = "java/data/Antonio-Bold.ttf";
                if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
                    fontPath = "../resources/fonts/Antonio-Bold.ttf";
                    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
                        std::cerr << "Erro ao carregar arquivo de fonte Antonio-Bold.ttf" << std::endl;
                        FT_Done_FreeType(ft);
                        return;
                    }
                }
            }
        }
    }

    FT_Set_Pixel_Sizes(face, 0, 24);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "Erro ao carregar glifo do caractere: " << c << std::endl;
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RED,
            face->glyph->bitmap.width, face->glyph->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            Vec2((float)face->glyph->bitmap.width, (float)face->glyph->bitmap.rows),
            Vec2((float)face->glyph->bitmap_left, (float)face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Cores baseadas nos nucleotideos
Vec3 getNucleotideColor(const std::string& base) {
    if (base == "A") return Vec3(0.906f, 0.298f, 0.290f);
    if (base == "T") return Vec3(0.227f, 0.553f, 0.878f);
    if (base == "G") return Vec3(0.165f, 0.667f, 0.416f);
    if (base == "C") return Vec3(0.941f, 0.753f, 0.251f);
    return Vec3(0.8f, 0.8f, 0.8f);
}

Vec3 getNucleotideBorderColor(const std::string& base) {
    if (base == "A") return Vec3(0.639f, 0.176f, 0.176f);
    if (base == "T") return Vec3(0.094f, 0.373f, 0.647f);
    if (base == "G") return Vec3(0.031f, 0.314f, 0.255f);
    if (base == "C") return Vec3(0.541f, 0.408f, 0.0f);
    return Vec3(0.5f, 0.5f, 0.5f);
}

Vec3 getNucleotideTextColor(const std::string& base) {
    if (base == "C") return Vec3(0.353f, 0.251f, 0.0f);
    return Vec3(1.0f, 1.0f, 1.0f);
}

void runEngineLoop() {
    if (g_Window == nullptr) return;

    // foca camera no primeiro ponto divergente na inicializacao sob lock
    {
        std::lock_guard<std::mutex> lock(g_RenderMutex);
        int posicao_div = -1;
        if (!g_RenderData.divergentPositions.empty()) {
            posicao_div = g_RenderData.divergentPositions[0];
        }
        if (posicao_div >= 0 && g_RenderData.cameraOffsetX == 0.0f) {
            float cx = posicao_div * ADVANCE_CODON + CODON_W / 2.0f;
            g_RenderData.cameraOffsetX = std::max(0.0f, cx - (SCR_WIDTH - 34.0f) / 2.0f);
        }
    }

    // Calcula escala do frame buffer para High-DPI
    int fb_w, fb_h;
    glfwGetFramebufferSize(g_Window, &fb_w, &fb_h);
    int win_w, win_h;
    glfwGetWindowSize(g_Window, &win_w, &win_h);
    float escala_x = (float)fb_w / (float)win_w;
    float escala_y = (float)fb_h / (float)win_h;

    float projection[16];
    setOrthoMatrix(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, -1.0f, 1.0f, projection);

    while (!glfwWindowShouldClose(g_Window)) {
        glClearColor(0.07f, 0.07f, 0.09f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // copia dados de renderizacao sob lock do mutex
        RenderData localData;
        {
            std::lock_guard<std::mutex> lock(g_RenderMutex);
            localData = g_RenderData;
        }

        std::vector<InstanceData> instances;

        int divIndex = -1;
        if (!localData.divergentPositions.empty()) {
            divIndex = localData.divergentPositions[0];
        }

        int indiceBaseInserida = localData.insertedBaseIndex;

        // Linha divisoria divergente no fundo
        if (divIndex >= 0) {
            float startX = divIndex * ADVANCE_CODON - 3.0f;
            
            InstanceData divider;
            divider.offset = Vec2(startX, 284.0f);
            divider.scale = Vec2(1.5f, 132.0f);
            divider.color = Vec3(0.886f, 0.294f, 0.290f);
            divider.borderColor = Vec3(0.886f, 0.294f, 0.290f);
            divider.shapeType = 0.0f;
            divider.dashed = 1.0f;
            instances.push_back(divider);
        }

        // Linhas de conexao dos aminoacidos na fita superior
        size_t nRef = localData.refAminoAcids.size();
        for (size_t i = 0; i < nRef; ++i) {
            float cx = i * ADVANCE_CODON + CODON_W / 2.0f;

            if (i < nRef - 1) {
                float next_cx = (i + 1) * ADVANCE_CODON + CODON_W / 2.0f;
                float lineLen = next_cx - cx - BALL_D;

                InstanceData link;
                link.offset = Vec2(cx + BALL_R + lineLen / 2.0f, AA_POS_Y_REF);
                link.scale = Vec2(lineLen, 1.5f);
                link.color = Vec3(0.2f, 0.65f, 0.45f);
                link.borderColor = link.color;
                link.shapeType = 2.0f;
                link.dashed = 0.0f;
                instances.push_back(link);
            }

            InstanceData verticalLink;
            float lineLen = FITA_Y_REF - (AA_POS_Y_REF + BALL_R);
            verticalLink.offset = Vec2(cx, AA_POS_Y_REF + BALL_R + lineLen / 2.0f);
            verticalLink.scale = Vec2(1.0f, lineLen);
            verticalLink.color = Vec3(0.2f, 0.65f, 0.45f);
            verticalLink.borderColor = verticalLink.color;
            verticalLink.shapeType = 0.0f;
            verticalLink.dashed = 1.0f;
            instances.push_back(verticalLink);
        }

        // Linhas de conexao dos aminoacidos na fita inferior
        size_t nMut = localData.mutAminoAcids.size();
        for (size_t i = 0; i < nMut; ++i) {
            float cx = i * ADVANCE_CODON + CODON_W / 2.0f;

            if (i < nMut - 1) {
                float next_cx = (i + 1) * ADVANCE_CODON + CODON_W / 2.0f;
                float lineLen = next_cx - cx - BALL_D;
                bool isDiv = (divIndex >= 0 && ((int)i >= divIndex || (int)(i + 1) >= divIndex));

                InstanceData link;
                link.offset = Vec2(cx + BALL_R + lineLen / 2.0f, AA_POS_Y_MUT);
                link.scale = Vec2(lineLen, 1.5f);
                link.color = isDiv ? Vec3(0.9f, 0.1f, 0.1f) : Vec3(0.2f, 0.65f, 0.45f);
                link.borderColor = link.color;
                link.shapeType = 2.0f;
                link.dashed = 0.0f;
                instances.push_back(link);
            }

            bool isDiv = (divIndex >= 0 && (int)i >= divIndex);
            InstanceData verticalLink;
            float lineLen = FITA_Y_MUT - (AA_POS_Y_MUT + BALL_R);
            verticalLink.offset = Vec2(cx, AA_POS_Y_MUT + BALL_R + lineLen / 2.0f);
            verticalLink.scale = Vec2(1.0f, lineLen);
            verticalLink.color = isDiv ? Vec3(0.9f, 0.1f, 0.1f) : Vec3(0.2f, 0.65f, 0.45f);
            verticalLink.borderColor = verticalLink.color;
            verticalLink.shapeType = 0.0f;
            verticalLink.dashed = 1.0f;
            instances.push_back(verticalLink);
        }

        // Renderizacao dos nucleotideos
        size_t nRefNuc = localData.refNucleotides.size();
        for (size_t j = 0; j < nRefNuc; ++j) {
            float cx = (j / 3) * ADVANCE_CODON + (j % 3) * (NUC_W + NUC_GAP) + NUC_W / 2.0f;
            InstanceData nuc;
            nuc.offset = Vec2(cx, FITA_Y_REF + NUC_H / 2.0f);
            nuc.scale = Vec2(NUC_W, NUC_H);
            nuc.color = getNucleotideColor(localData.refNucleotides[j]);
            nuc.borderColor = getNucleotideBorderColor(localData.refNucleotides[j]);
            nuc.shapeType = 0.0f;
            nuc.dashed = 0.0f;
            instances.push_back(nuc);
        }

        size_t nMutNuc = localData.mutNucleotides.size();
        for (size_t j = 0; j < nMutNuc; ++j) {
            float cx = (j / 3) * ADVANCE_CODON + (j % 3) * (NUC_W + NUC_GAP) + NUC_W / 2.0f;
            bool isInsertedBase = (indiceBaseInserida >= 0 && j == (size_t)indiceBaseInserida);

            InstanceData nuc;
            nuc.offset = Vec2(cx, FITA_Y_MUT + NUC_H / 2.0f);
            nuc.scale = Vec2(NUC_W, NUC_H);
            nuc.color = isInsertedBase ? Vec3(0.988f, 0.922f, 0.922f) : getNucleotideColor(localData.mutNucleotides[j]);
            nuc.borderColor = isInsertedBase ? Vec3(0.9f, 0.1f, 0.1f) : getNucleotideBorderColor(localData.mutNucleotides[j]);
            nuc.shapeType = 0.0f;
            nuc.dashed = isInsertedBase ? 1.0f : 0.0f;
            instances.push_back(nuc);
        }

        // Renderizacao das bolinhas dos aminoacidos
        for (size_t i = 0; i < nRef; ++i) {
            float cx = i * ADVANCE_CODON + CODON_W / 2.0f;
            InstanceData ball;
            ball.offset = Vec2(cx, AA_POS_Y_REF);
            ball.scale = Vec2(BALL_D, BALL_D);
            ball.color = Vec3(0.8f, 0.93f, 0.88f); // Aumentado contraste do verde claro
            ball.borderColor = Vec3(0.114f, 0.620f, 0.459f);
            ball.shapeType = 1.0f;
            ball.dashed = 0.0f;
            instances.push_back(ball);
        }

        for (size_t i = 0; i < nMut; ++i) {
            float cx = i * ADVANCE_CODON + CODON_W / 2.0f;
            bool isDiv = (divIndex >= 0 && (int)i >= divIndex);
            bool isLastEqual = (divIndex >= 0 && (int)i == divIndex - 1);

            InstanceData ball;
            ball.offset = Vec2(cx, AA_POS_Y_MUT);
            ball.scale = Vec2(BALL_D, BALL_D);
            
            if (isLastEqual) {
                ball.color = Vec3(1.0f, 0.953f, 0.804f);
                ball.borderColor = Vec3(0.941f, 0.753f, 0.251f);
            } else if (isDiv) {
                ball.color = Vec3(0.886f, 0.294f, 0.290f); 
                ball.borderColor = Vec3(0.886f, 0.294f, 0.290f);
            } else {
                ball.color = Vec3(0.8f, 0.93f, 0.88f); // Aumentado contraste do verde claro
                ball.borderColor = Vec3(0.114f, 0.620f, 0.459f);
            }
            ball.shapeType = 1.0f;
            ball.dashed = 0.0f;
            instances.push_back(ball);
        }

        // Envio e renderizacao dos cards de fundo 
        std::vector<InstanceData> cardsFundo;
        
        InstanceData cardRef;
        cardRef.offset = Vec2(500.0f, 84.0f);
        cardRef.scale = Vec2(980.0f, 164.0f);
        cardRef.color = Vec3(1.0f, 1.0f, 1.0f);
        cardRef.borderColor = Vec3(1.0f, 1.0f, 1.0f);
        cardRef.shapeType = 5.0f;
        cardRef.dashed = 0.0f;
        cardsFundo.push_back(cardRef);

        InstanceData headerRef;
        headerRef.offset = Vec2(500.0f, 18.0f);
        headerRef.scale = Vec2(980.0f, 32.0f);
        headerRef.color = Vec3(0.85f, 0.94f, 0.90f);
        headerRef.borderColor = headerRef.color;
        headerRef.shapeType = 4.0f;
        headerRef.dashed = 0.0f;
        cardsFundo.push_back(headerRef);

        InstanceData cardMut;
        cardMut.offset = Vec2(500.0f, 268.0f);
        cardMut.scale = Vec2(980.0f, 164.0f);
        cardMut.color = Vec3(1.0f, 1.0f, 1.0f);
        cardMut.borderColor = Vec3(1.0f, 1.0f, 1.0f);
        cardMut.shapeType = 5.0f;
        cardMut.dashed = 0.0f;
        cardsFundo.push_back(cardMut);

        InstanceData headerMut;
        headerMut.offset = Vec2(500.0f, 202.0f);
        headerMut.scale = Vec2(980.0f, 32.0f);
        headerMut.color = Vec3(0.97f, 0.90f, 0.90f);
        headerMut.borderColor = headerMut.color;
        headerMut.shapeType = 4.0f;
        headerMut.dashed = 0.0f;
        cardsFundo.push_back(headerMut);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(InstanceData) * cardsFundo.size(), cardsFundo.data());

        glUseProgram(shapeShader);

        float viewIdentidade[16];
        setTranslationMatrix(0.0f, 0.0f, viewIdentidade);

        glUniformMatrix4fv(shapeProjLoc, 1, GL_FALSE, projection);
        glUniformMatrix4fv(shapeViewLoc, 1, GL_FALSE, viewIdentidade);

        glBindVertexArray(shapeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(cardsFundo.size()));

        // Envio e renderizacao das geometrias dinamicas da sequencia
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(InstanceData) * instances.size(), instances.data());

        float view[16];
        setTranslationMatrix(34.0f - localData.cameraOffsetX, 0.0f, view);
        glUniformMatrix4fv(shapeViewLoc, 1, GL_FALSE, view);

        glEnable(GL_SCISSOR_TEST);
        glScissor((int)(18.0f * escala_x), 0, (int)(964.0f * escala_x), fb_h);

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, static_cast<GLsizei>(instances.size()));
        glBindVertexArray(0);

        glDisable(GL_SCISSOR_TEST);

        // renderizacao de textos
        glUseProgram(textShader);
        glUniformMatrix4fv(textProjLoc, 1, GL_FALSE, projection);
        glUniformMatrix4fv(textViewLoc, 1, GL_FALSE, view);

        // ativa Scissor Test para os textos da sequencia que rola
        glEnable(GL_SCISSOR_TEST);
        glScissor((int)(18.0f * escala_x), 0, (int)(964.0f * escala_x), fb_h);

        // letras fita superior
        for (size_t j = 0; j < nRefNuc; ++j) {
            float cx = (j / 3) * ADVANCE_CODON + (j % 3) * (NUC_W + NUC_GAP);
            std::string nucStr = localData.refNucleotides[j];
            float nucScale = 0.58f;
            char nucChar = localData.refNucleotides[j][0];
            Character ch = Characters[nucChar];
            float nucX = cx + (NUC_W - (ch.Advance >> 6) * nucScale) / 2.0f;
            float nucY = FITA_Y_REF + NUC_H / 2.0f + (ch.Bearing.y * nucScale) / 2.0f;
            RenderText(nucStr, nucX, nucY, nucScale, getNucleotideTextColor(nucStr), true);
            
            int val = j + 1;
            float numScale = 0.48f;
            std::string numStr = std::to_string(val);
            float numX = cx + (NUC_W / 2.0f) - obterLarguraTexto(numStr, numScale) / 2.0f;
            RenderText(numStr, numX, FITA_Y_REF + NUC_H + 14.0f, numScale, Vec3(0.27f, 0.27f, 0.27f), true);
        }

        // letras fita inferior
        for (size_t j = 0; j < nMutNuc; ++j) {
            float cx = (j / 3) * ADVANCE_CODON + (j % 3) * (NUC_W + NUC_GAP);
            bool isInsertedBase = (indiceBaseInserida >= 0 && j == (size_t)indiceBaseInserida);
            Vec3 txtCol = isInsertedBase ? Vec3(0.9f, 0.1f, 0.1f) : getNucleotideTextColor(localData.mutNucleotides[j]);
            
            std::string nucStr = localData.mutNucleotides[j];
            float nucScale = 0.58f;
            char nucChar = localData.mutNucleotides[j][0];
            Character ch = Characters[nucChar];
            float nucX = cx + (NUC_W - (ch.Advance >> 6) * nucScale) / 2.0f;
            float nucY = FITA_Y_MUT + NUC_H / 2.0f + (ch.Bearing.y * nucScale) / 2.0f;
            RenderText(nucStr, nucX, nucY, nucScale, txtCol, true);
            
            bool isDiv = (divIndex >= 0 && j >= (size_t)divIndex * 3);
            Vec3 numCol = isDiv ? Vec3(0.886f, 0.294f, 0.290f) : Vec3(0.27f, 0.27f, 0.27f);
            
            int val = j + 1;
            float numScale = 0.48f;
            std::string numStr = std::to_string(val);
            float numX = cx + (NUC_W / 2.0f) - obterLarguraTexto(numStr, numScale) / 2.0f;
            RenderText(numStr, numX, FITA_Y_MUT + NUC_H + 14.0f, numScale, numCol, true);
        }

        // aminoacidos fita superior
        for (size_t i = 0; i < nRef; ++i) {
            float cx = i * ADVANCE_CODON + CODON_W / 2.0f;
            
            char aaChar = localData.refAminoAcids[i][0];
            Character ch = Characters[aaChar];
            float txtScale = 0.58f;
            float text_w = (ch.Advance >> 6) * txtScale;
            float xPos = cx - (text_w / 2.0f);
            float yPos = AA_POS_Y_REF + (ch.Bearing.y - ch.Size.y / 2.0f) * txtScale;
            
            RenderText(localData.refAminoAcids[i], xPos, yPos, txtScale, Vec3(0.031f, 0.314f, 0.255f), true);
            
            int val = i + 1;
            float numScale = 0.48f;
            std::string numStr = std::to_string(val);
            float numX = cx - obterLarguraTexto(numStr, numScale) / 2.0f;
            RenderText(numStr, numX, AA_POS_Y_REF - BALL_R - 5.0f, numScale, Vec3(0.3f, 0.3f, 0.3f), true);
        }

        // aminoacidos fita inferior
        for (size_t i = 0; i < nMut; ++i) {
            float cx = i * ADVANCE_CODON + CODON_W / 2.0f;
            bool isDiv = (divIndex >= 0 && (int)i >= divIndex);
            bool isLastEqual = (divIndex >= 0 && (int)i == divIndex - 1);
            
            Vec3 txtCol;
            if (isLastEqual) {
                txtCol = Vec3(0.478f, 0.345f, 0.0f);
            } else if (isDiv) {
                txtCol = Vec3(1.0f, 1.0f, 1.0f);
            } else {
                txtCol = Vec3(0.031f, 0.314f, 0.255f);
            }

            char aaChar = localData.mutAminoAcids[i][0];
            Character ch = Characters[aaChar];
            float txtScale = 0.58f;
            float text_w = (ch.Advance >> 6) * txtScale;
            float xPos = cx - (text_w / 2.0f);
            float yPos = AA_POS_Y_MUT + (ch.Bearing.y - ch.Size.y / 2.0f) * txtScale;

            RenderText(localData.mutAminoAcids[i], xPos, yPos, txtScale, txtCol, true);
            
            int val = i + 1;
            float numScale = 0.48f;
            std::string numStr = std::to_string(val);
            float numX = cx - obterLarguraTexto(numStr, numScale) / 2.0f;
            RenderText(numStr, numX, AA_POS_Y_MUT - BALL_R - 5.0f, numScale, isDiv ? Vec3(0.886f, 0.294f, 0.290f) : Vec3(0.3f, 0.3f, 0.3f), true);
        }

        glDisable(GL_SCISSOR_TEST);

        // textos dos headers
        std::string txtRef = "REFERENCIA - " + localData.transcriptId + " - " + std::to_string(nRef) + " aminoacidos";
        RenderText(txtRef, 30.0f, 25.0f, 0.6f, Vec3(0.15f, 0.40f, 0.30f), false);

        std::string txtMut = "MUTANTE - " + localData.mutationHgvs + " - frameshift - " + std::to_string(nMut) + " aminoacidos";
        RenderText(txtMut, 30.0f, 209.0f, 0.6f, Vec3(0.65f, 0.20f, 0.20f), false);

        glfwSwapBuffers(g_Window);
        glfwPollEvents();
    }
}

void cleanupEngine() {
    if (g_Window) {
        glfwDestroyWindow(g_Window);
        g_Window = nullptr;
    }
    if (shapeVAO) {
        glDeleteVertexArrays(1, &shapeVAO);
        glDeleteBuffers(1, &shapeVBO);
        glDeleteBuffers(1, &instanceVBO);
    }
    if (textVAO) {
        glDeleteVertexArrays(1, &textVAO);
        glDeleteBuffers(1, &textVBO);
    }
    for (auto const& [key, val] : Characters) {
        glDeleteTextures(1, &val.TextureID);
    }
    Characters.clear();

    if (shapeShader) glDeleteProgram(shapeShader);
    if (textShader) glDeleteProgram(textShader);

    glfwTerminate();
}

#ifdef SHIFTGL_STANDALONE
int main() {
    std::cout << "[STANDALONE] Rodando ShiftGL em modo de teste estatico..." << std::endl;
    g_RenderData.refNucleotides = {
        "A", "T", "G", "C", "T", "T", "C", "A", "G", "A", "C", "C", "G", "C", "A", "G"
    };
    g_RenderData.mutNucleotides = {
        "A", "T", "G", "C", "T", "T", "C", "A", "G", "A", "C", "C", "A", "G", "C", "A", "G"
    };
    g_RenderData.refAminoAcids = {"M", "L", "Q", "T", "A"};
    g_RenderData.mutAminoAcids = {"M", "L", "Q", "T", "S"};
    g_RenderData.divergentPositions = {4};
    g_RenderData.insertedBaseIndex = 12;
    g_RenderData.transcriptId = "NM_001037732.3";
    g_RenderData.mutationHgvs = "c.183_184insA";

    initEngine();
    runEngineLoop();
    cleanupEngine();
    return 0;
}
#endif
