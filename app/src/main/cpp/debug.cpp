#include "debug.h"

void printDebug(string print) {
    #if debug
        Serial.print(print);
    #endif
}

void printDebug(int print) {
    #if debug
        Serial.print(print);
    #endif
}

void printDebug(unsigned int print) {
    #if debug
        Serial.print(print);
    #endif
}

void printDebug(float print) {
    #if debug
        Serial.print(print);
    #endif
}

void printDebug(double print) {
    #if debug
        Serial.print(print);
    #endif
}

void printDebugLn(string print) {
    #if debug
        Serial.println(print);
    #endif
}

void printDebugLn(int print) {
    #if debug
        Serial.println(print);
    #endif
}

void printDebugLn(unsigned int print) {
    #if debug
        Serial.println(print);
    #endif
}

void printDebugLn(float print) {
    #if debug
        Serial.println(print);
    #endif
}

void printDebugLn(double print) {
    #if debug
        Serial.println(print);
    #endif
}