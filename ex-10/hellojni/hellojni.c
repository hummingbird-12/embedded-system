#include "HelloJni.h"

JNIEXPORT void JNICALL Java_HelloJni_printHello(JNIEnv *env, jobject obj) {
        printf("hello\n");
}

JNIEXPORT void JNICALL Java_HelloJni_printString(JNIEnv *env, jobject obj, jstring string) {
        const char *str = (*env)->GetStringUTFChars(env, string, 0);
        printf("%s\n", str);
}

