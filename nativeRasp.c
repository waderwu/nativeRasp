#include <stdio.h>
#include <string.h>
#include <jni.h>

int startsWith(const char *str, char *prefix) {
    int len1 = strlen(str);
    int len2 = strlen(prefix);

    if (len1 < len2) return 0;
    
    return strncmp(str, prefix, len2) == 0;
}

int containsDots(const char *str) {
    int len = strlen(str);
    for (int i = 0; i < len - 1; i++) {
        if (str[i] == '.' && str[i + 1] == '.') {
            return 1;
        }
    }
    return 0;
}



void * oldLoad;

JNIEXPORT jint JNICALL wuforkAndExec
  (JNIEnv * env, jobject process, jint mode, jbyteArray helperpath, jbyteArray prog, jbyteArray argblock, jint argc, jbyteArray envblock, jint envc, jbyteArray dir, jintArray std_fds, jboolean redirectErrorStream){
	jbyte* progPtr = (*env)->GetByteArrayElements(env, prog, NULL);
	char* charProgPtr = (char*)progPtr;
	printf("prog: %s\n", charProgPtr);

	jbyte* argblockPtr = (*env)->GetByteArrayElements(env, argblock, NULL);
	char* charArgblockPtr = (char*)argblockPtr;
	printf("argblock: %s\n", charArgblockPtr);

	(*env)->ReleaseByteArrayElements(env, prog, progPtr, 0);
	(*env)->ReleaseByteArrayElements(env, argblock, argblockPtr, 0);
	return 0;
}

JNIEXPORT void JNICALL wuload
  (JNIEnv * env, jobject obj, jstring name, jboolean isBuiltin){
	const char* strName = (*env)->GetStringUTFChars(env, name, NULL);
	printf("lib name: %s\n", strName);
    if (!containsDots(strName) && startsWith(strName, "/usr/local/openjdk-8/jre/lib/amd64/")){
        ((void (*)(JNIEnv *, jobject, jstring, jboolean))oldLoad)(env, obj, name, isBuiltin);
    }
	(*env)->ReleaseStringUTFChars(env, name, strName);
}

static JNINativeMethod processMethods[] = {
	{ "forkAndExec", "(I[B[B[BI[BI[B[IZ)I", (void *) wuforkAndExec}
};

static JNINativeMethod nativeLibraryMethods[] = {
	{ "load", "(Ljava/lang/String;Z)V", (void *) wuload}
};

static jclass processClass;
static jclass nativeLibraryClass;

static const char* const processClassName = "java/lang/UNIXProcess";
static const char* const nativeLibraryClassName = "java/lang/ClassLoader$NativeLibrary";



JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  printf("jni onload\n");
	JNIEnv* env = NULL;
	jint result = -1;
	if ((*vm)->GetEnv(vm, (void **)&env, JNI_VERSION_1_4) != JNI_OK) { 
		return -1;
	}
	
	processClass = (*env)->FindClass(env, processClassName);
	nativeLibraryClass = (*env)->FindClass(env, nativeLibraryClassName);

    jmethodID method = (*env)->GetMethodID(env, nativeLibraryClass, "load", "(Ljava/lang/String;Z)V");

    oldLoad = *(void **)((*((void **)method))+88);

	if (processClass == NULL)
	{
		printf("cannot get class:%s\n", processClassName);
		return -1;
	}

    if (nativeLibraryClass == NULL)
	{
		printf("cannot get class:%s\n", nativeLibraryClassName);
		return -1;
	}

	if ((*env)->RegisterNatives(env, processClass, processMethods, sizeof(processMethods) / sizeof(processMethods[0])) < 0)
	{
		printf("register native processMethods failed!\n");
		return -1;
	}

	if ((*env)->RegisterNatives(env, nativeLibraryClass, nativeLibraryMethods, sizeof(nativeLibraryMethods) / sizeof(nativeLibraryMethods[0])) < 0)
	{
		printf("register native nativeLibraryMethods failed!\n");
		return -1;
	}

	return JNI_VERSION_1_4;
}
