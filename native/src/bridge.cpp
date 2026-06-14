#include "jni_OpenGLBridge.h"
#include "engine.h"
#include <iostream>
#include <vector>
#include <string>

JNIEXPORT void JNICALL Java_jni_OpenGLBridge_sendData(
    JNIEnv *env, jclass clazz,
    jobjectArray refNuc, jobjectArray mutNuc,
    jobjectArray refAA, jobjectArray mutAA,
    jintArray divPos, jint insertedBaseIndex,
    jstring transcriptId, jstring mutationHgvs
) {
    std::cout << "[JNI-C++] Recebendo dados do Java..." << std::endl;

    // Converte jobjectArray (String[] em Java) para std::vector<std::string>
    auto convertStringArray = [&](jobjectArray arr, std::vector<std::string>& vec) {
        vec.clear();
        if (!arr) return;
        jsize len = env->GetArrayLength(arr);
        vec.reserve(len);
        for (jsize i = 0; i < len; ++i) {
            jstring jstr = (jstring)env->GetObjectArrayElement(arr, i);
            if (jstr) {
                const char* chars = env->GetStringUTFChars(jstr, nullptr);
                if (chars) {
                    vec.push_back(std::string(chars));
                    env->ReleaseStringUTFChars(jstr, chars);
                }
            } else {
                vec.push_back("N");
            }
        }
    };

    convertStringArray(refNuc, g_RenderData.refNucleotides);
    convertStringArray(mutNuc, g_RenderData.mutNucleotides);
    convertStringArray(refAA, g_RenderData.refAminoAcids);
    convertStringArray(mutAA, g_RenderData.mutAminoAcids);

    // Converte jintArray (int[] em Java) para std::vector<int>
    g_RenderData.divergentPositions.clear();
    if (divPos) {
        jsize len = env->GetArrayLength(divPos);
        g_RenderData.divergentPositions.reserve(len);
        jint* body = env->GetIntArrayElements(divPos, nullptr);
        if (body) {
            for (jsize i = 0; i < len; ++i) {
                g_RenderData.divergentPositions.push_back(body[i]);
            }
            env->ReleaseIntArrayElements(divPos, body, JNI_ABORT);
        }
    }

    g_RenderData.insertedBaseIndex = insertedBaseIndex;

    if (transcriptId) {
        const char* chars = env->GetStringUTFChars(transcriptId, nullptr);
        if (chars) {
            g_RenderData.transcriptId = std::string(chars);
            env->ReleaseStringUTFChars(transcriptId, chars);
        }
    } else {
        g_RenderData.transcriptId = "";
    }

    if (mutationHgvs) {
        const char* chars = env->GetStringUTFChars(mutationHgvs, nullptr);
        if (chars) {
            g_RenderData.mutationHgvs = std::string(chars);
            env->ReleaseStringUTFChars(mutationHgvs, chars);
        }
    } else {
        g_RenderData.mutationHgvs = "";
    }

    std::cout << "[JNI-C++] Dados recebidos com sucesso:" << std::endl;
    std::cout << "  - Bases Ref: " << g_RenderData.refNucleotides.size() << std::endl;
    std::cout << "  - Bases Mut: " << g_RenderData.mutNucleotides.size() << std::endl;
    std::cout << "  - Aminoacidos Ref: " << g_RenderData.refAminoAcids.size() << std::endl;
    std::cout << "  - Aminoacidos Mut: " << g_RenderData.mutAminoAcids.size() << std::endl;
    std::cout << "  - Posicoes Divergentes: " << g_RenderData.divergentPositions.size() << std::endl;
    std::cout << "  - Indice Base Inserida: " << g_RenderData.insertedBaseIndex << std::endl;
    std::cout << "  - Transcrito: " << g_RenderData.transcriptId << std::endl;
    std::cout << "  - Mutacao: " << g_RenderData.mutationHgvs << std::endl;
}

JNIEXPORT void JNICALL Java_jni_OpenGLBridge_openWindow(JNIEnv *env, jclass clazz) {
    std::cout << "[JNI-C++] Abrindo a janela gráfica GLFW/OpenGL..." << std::endl;
    initEngine();
    runEngineLoop();
    cleanupEngine();
    std::cout << "[JNI-C++] Janela gráfica encerrada." << std::endl;
}

JNIEXPORT void JNICALL Java_jni_OpenGLBridge_moveCamera(JNIEnv *env, jclass clazz, jfloat offset) {
    g_RenderData.cameraOffsetX = offset;
}
