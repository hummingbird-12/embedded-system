#include <stdio.h>
//#include "JniFuncMain.h"
//#include "JniTest.h"
#include <jni.h>

JNIEXPORT jobject JNICALL Java_JniFuncMain_createJniObject(JNIEnv* env, jclass clazz) {
	jfieldID fid = (*env)->GetStaticFieldID( env, clazz, "staticIntField", "I" );
	jint staticIntField = (*env)->GetStaticIntField(env, clazz,  fid);

	printf("native staticIntfiled %d\n", staticIntField );

	jclass targetClass = (*env)->FindClass( env, "JniTest");
	jmethodID mid = (*env)->GetMethodID(env, targetClass, "<init>", "(I)V");

	jobject newObject = (*env)->NewObject( env, targetClass, mid, 100 );

	mid = (*env)->GetMethodID(env, targetClass, "callByNative", "(I)I" );
	int result = (*env)->CallIntMethod( env, newObject, mid, 200 );
	printf("native result %d\n", result);

	fid = (*env)->GetFieldID( env, targetClass, "intField", "I");
	(*env)->SetIntField( env, newObject, fid, result );

	return newObject;
}
JNIEXPORT void JNICALL Java_JniTest_callNative2(JNIEnv* env, jobject clazz, jint num) {
	printf("native callNative2\n");

	jclass targetClass = (*env)->GetObjectClass( env,  clazz ); 
	jmethodID mid = (*env)->GetMethodID(env, targetClass, "callByNative2", "(I)V");

	(*env)->CallVoidMethod( env, clazz, mid, num+5);
}
