#include <jni.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "android/log.h"
#include "driver_specs.h"

#define DEVICE_FILE(X) ("/dev/" DEVICE_NAME)
#define STATUS_EXIT -1
#define STATUS_INPUT 0
#define STATUS_GUESSED 1
#define WORD_MAX_LEN 20

#define LOG_TAG "HANGMAN"
#define LOGV(...)   __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)

/*
 * Opens device file and returns the file descriptor.
 */
jint JNICALL Java_com_example_hangman_MainActivity_openDevice(JNIEnv *env,
		jobject this) {
	int fd = open(DEVICE_FILE(DEVICE_NAME), O_RDWR);

	if (fd == -1) {
		LOGV("Error opening device file\n");
		return -1;
	}
	return fd;
}

/*
 * Calls `ioctl()` to wait for Module's input and `read()` to get the data from the Module.
 */
void JNICALL Java_com_example_hangman_MainActivity_startHangman(JNIEnv *env,
		jobject this, jint fd, jobject ret) {
	struct _payload {
		char word[WORD_MAX_LEN];
		int status;
		int score;
	} payload;

	jclass retClass = (*env)->GetObjectClass(env, ret);
	jfieldID jwordID = (*env)->GetFieldID(env, retClass, "word", "Ljava/lang/String;");
	jfieldID jstatusID = (*env)->GetFieldID(env, retClass, "status", "I");
	jfieldID jscoreID = (*env)->GetFieldID(env, retClass, "score", "I");

	jstring word;

	ioctl(fd, IOCTL_READ_LETTER);
	read(fd, &payload, sizeof(payload));

	word = (*env)->NewStringUTF(env, payload.word);

	switch (payload.status) {
	case STATUS_INPUT:
		LOGV("[native] input: %s", payload.word);

		(*env)->SetIntField(env, ret, jstatusID, payload.status);
		(*env)->SetObjectField(env, ret, jwordID, word);
		break;
	case STATUS_GUESSED:
		LOGV("[native] guess word: %s", payload.word);
		LOGV("[native] guess score: %d", payload.score);

		(*env)->SetIntField(env, ret, jstatusID, payload.status);
		(*env)->SetObjectField(env, ret, jwordID, word);
		(*env)->SetIntField(env, ret, jscoreID, payload.score);
		break;
	case STATUS_EXIT:
		LOGV("[native] exit");

		(*env)->SetIntField(env, ret, jstatusID, payload.status);
		break;
	default:
		LOGV("UNKNOWN STATUS");
		break;
	}

	return;
}

/*
 * Closes device file.
 */
void JNICALL Java_com_example_hangman_MainActivity_closeDevice(JNIEnv *env, jobject this, jint fd) {
	close(fd);
}
