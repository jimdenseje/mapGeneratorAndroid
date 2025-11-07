#include <string>
#include <android/log.h>

#define LOGV(TAG, ...) __android_log_print(ANDROID_LOG_VERBOSE, TAG,__VA_ARGS__)
#define LOGD(TAG, ...) __android_log_print(ANDROID_LOG_DEBUG  , TAG,__VA_ARGS__)
#define LOGI(TAG, ...) __android_log_print(ANDROID_LOG_INFO   , TAG,__VA_ARGS__)
#define LOGW(TAG, ...) __android_log_print(ANDROID_LOG_WARN   , TAG,__VA_ARGS__)
#define LOGE(TAG, ...) __android_log_print(ANDROID_LOG_ERROR  , TAG,__VA_ARGS__)

using namespace std;

#define debug false

void printDebug(string print);
void printDebug(int print);
void printDebug(unsigned int print);
void printDebug(float print);
void printDebug(double print);

void printDebugLn(string print);
void printDebugLn(int print);
void printDebugLn(unsigned int print);
void printDebugLn(float print);
void printDebugLn(double print);