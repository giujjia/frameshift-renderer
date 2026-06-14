#include "jni_OpenGLBridge.h"
#include "engine.h"
#include <iostream>
#include <vector>
#include <string>
#include <mutex>

JNIEXPORT void JNICALL Java_jni_OpenGLBridge_sendData(
    JNIEnv *env, jclass clazz,
    jobjectArray refNuc, jobjectArray mutNuc,
    jobjectArray refAA, jobjectArray mutAA,
    jintArray divPos, jint insertedBaseIndex,
    jstring transcriptId, jstring mutationHgvs
) {
    std::cout << "[JNI-C++] Recebendo dados do Java..." << std::endl;

    std::vector<std::string> localRefNuc, localMutNuc, localRefAA, localMutAA;
    std::vector<int> localDivPos;
    std::string localTranscriptId, localMutationHgvs;

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

    convertStringArray(refNuc, localRefNuc);
    convertStringArray(mutNuc, localMutNuc);
    convertStringArray(refAA, localRefAA);
    convertStringArray(mutAA, localMutAA);

    if (divPos) {
        jsize len = env->GetArrayLength(divPos);
        localDivPos.reserve(len);
        jint* body = env->GetIntArrayElements(divPos, nullptr);
        if (body) {
            for (jsize i = 0; i < len; ++i) {
                localDivPos.push_back(body[i]);
            }
            env->ReleaseIntArrayElements(divPos, body, JNI_ABORT);
        }
    }

    if (transcriptId) {
        const char* chars = env->GetStringUTFChars(transcriptId, nullptr);
        if (chars) {
            localTranscriptId = std::string(chars);
            env->ReleaseStringUTFChars(transcriptId, chars);
        }
    }

    if (mutationHgvs) {
        const char* chars = env->GetStringUTFChars(mutationHgvs, nullptr);
        if (chars) {
            localMutationHgvs = std::string(chars);
            env->ReleaseStringUTFChars(mutationHgvs, chars);
        }
    }

    // atualiza dados globais sob protecao do mutex
    {
        std::lock_guard<std::mutex> lock(g_RenderMutex);
        g_RenderData.refNucleotides = std::move(localRefNuc);
        g_RenderData.mutNucleotides = std::move(localMutNuc);
        g_RenderData.refAminoAcids = std::move(localRefAA);
        g_RenderData.mutAminoAcids = std::move(localMutAA);
        g_RenderData.divergentPositions = std::move(localDivPos);
        g_RenderData.insertedBaseIndex = insertedBaseIndex;
        g_RenderData.transcriptId = std::move(localTranscriptId);
        g_RenderData.mutationHgvs = std::move(localMutationHgvs);
    }

    std::cout << "[JNI-C++] Dados recebidos com sucesso" << std::endl;
}

JNIEXPORT void JNICALL Java_jni_OpenGLBridge_openWindow(JNIEnv *env, jclass clazz) {
    std::cout << "[JNI-C++] Abrindo a janela grafica GLFW/OpenGL..." << std::endl;
    initEngine();
    runEngineLoop();
    cleanupEngine();
    std::cout << "[JNI-C++] Janela grafica encerrada" << std::endl;
}

JNIEXPORT void JNICALL Java_jni_OpenGLBridge_moveCamera(JNIEnv *env, jclass clazz, jfloat offset) {
    std::lock_guard<std::mutex> lock(g_RenderMutex);
    g_RenderData.cameraOffsetX = offset;
}
