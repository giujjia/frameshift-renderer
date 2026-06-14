#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <string>

// dados de renderização compartilhados entre JNI e o motor gráfico
struct RenderData {
    std::vector<std::string> refNucleotides;
    std::vector<std::string> mutNucleotides;
    std::vector<std::string> refAminoAcids;
    std::vector<std::string> mutAminoAcids;
    std::vector<int> divergentPositions;
    float cameraOffsetX = 0.0f;
    int insertedBaseIndex = -1;
    std::string transcriptId;
    std::string mutationHgvs;
};

extern RenderData g_RenderData;

void initEngine();
void runEngineLoop();
void cleanupEngine();

#endif
