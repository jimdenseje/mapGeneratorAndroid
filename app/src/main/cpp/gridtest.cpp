#include <jni.h>
#include <cstdlib>
#include <thread>
#include <cstdio>
#include <iostream>
#include "game.h"

using namespace std;

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_gridtest_CompanionClass_00024Companion_initScreenRes(JNIEnv *env, jobject thiz,
    jint h, jint w
) {

    string buffer;
    for (int y = CANVAS_HEIGHT - 1; y >= 0; y--) {
        for (int x = 0; x < CANVAS_WIDTH; x++) {
            buffer += " " + to_string(mainMap[y * CANVAS_WIDTH + x]);
        }
        buffer += "\n\r";
    }
    //string data = ("hallo from cpp your sum is: " + to_string(h + w));
    jstring dataReturn = (*env).NewStringUTF(buffer.c_str());
    return dataReturn;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_gridtest_CompanionClass_00024Companion_getPixelColor(
    JNIEnv *env,
    jobject
    thiz,
    jint x,
    jint y
) {
    //int r = (colors[mainMap[y * CANVAS_WIDTH + x]] >> 16) & 0xFF;
    //int g = (colors[mainMap[y * CANVAS_WIDTH + x]] >> 8) & 0xFF;
    //int b = colors[mainMap[y * CANVAS_WIDTH + x]] & 0xFF;
    return colors[mainMap[y * CANVAS_WIDTH + x]];
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_gridtest_CompanionClass_00024Companion_init(JNIEnv *env, jobject thiz) {
    setupGame();
    initGame();
    //std::thread printScreenThread(initGame);
    return (*env).NewStringUTF("Game Has been Started");
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_gridtest_CompanionClass_00024Companion_gameLoop(JNIEnv *env, jobject thiz) {
    //movePlayerUserInput(1, 1);
    movePlayerFastAuto(1,1);
    return nullptr;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_gridtest_CompanionClass_00024Companion_goToNextLevel(JNIEnv *env, jobject thiz) {
    nextLevel = true;
    return nullptr;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_gridtest_CompanionClass_00024Companion_isMapLocked(JNIEnv *env, jobject thiz) {
    return mainMapLocked;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_gridtest_CompanionClass_00024Companion_lockMap(JNIEnv *env, jobject thiz) {
    mainMapLocked = true;
    return nullptr;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_gridtest_CompanionClass_00024Companion_unLockMap(JNIEnv *env, jobject thiz) {
    mainMapLocked = false;
    return nullptr;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_gridtest_CompanionClass_00024Companion_buttonPress(JNIEnv *env, jobject thiz,
                                                                  jint key) {
    keyStates[key] = true;
    return nullptr;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_gridtest_CompanionClass_00024Companion_getCanvasWidth(JNIEnv *env, jobject thiz) {
    return CANVAS_WIDTH;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_gridtest_CompanionClass_00024Companion_getCanvasHeight(JNIEnv *env, jobject thiz) {
    return CANVAS_HEIGHT;
}