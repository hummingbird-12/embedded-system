/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_example_hangman_MainActivity */

#ifndef _Included_com_example_hangman_MainActivity
#define _Included_com_example_hangman_MainActivity
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_example_hangman_MainActivity
 * Method:    openDevice
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_example_hangman_MainActivity_openDevice
  (JNIEnv *, jobject);

/*
 * Class:     com_example_hangman_MainActivity
 * Method:    startHangman
 * Signature: (ILcom/example/hangman/MainActivity/Payload;)V
 */
JNIEXPORT void JNICALL Java_com_example_hangman_MainActivity_startHangman
  (JNIEnv *, jobject, jint, jobject);

/*
 * Class:     com_example_hangman_MainActivity
 * Method:    closeDevice
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_example_hangman_MainActivity_closeDevice
  (JNIEnv *, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif
