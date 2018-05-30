#include <jni.h>
#include <string>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <malloc.h>

#include "UIMarkup.h"
extern "C"{

JNIEXPORT jstring JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_stringFromJNI(JNIEnv *env, jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT jlong JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_LaodXml(JNIEnv *env, jobject /* this */, jobject assetManager, jstring file) {
    char* pFile = (char*)env->GetStringUTFChars(file, 0);
    CMarkup* pMarkup = new CMarkup();
    if (pMarkup->LoadFromFile(env, assetManager, pFile)) {
        env->ReleaseStringUTFChars(file, pFile);
        return (jlong) pMarkup;
    }
    delete pMarkup;

    env->ReleaseStringUTFChars(file, pFile);
    return 0;
}

JNIEXPORT void JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_ReleaseXml(JNIEnv *env, jobject /* this */, jlong handle) {
    if (0 == handle) return;

    CMarkup* pMarkup = (CMarkup*)handle;
    delete pMarkup;
}

JNIEXPORT jlong JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_GetRoot(JNIEnv *env, jobject /* this */, jlong handle) {
    if (0 == handle) return 0;
    CMarkup* pXml = (CMarkup*)handle;
    return (jlong)pXml->GetRoot();
}

JNIEXPORT void JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_NodeRelease(JNIEnv *env, jobject /* this */, jlong node) {
    if (0 == node) return;
    CMarkupNode* pNode = (CMarkupNode*)node;
    delete pNode;
}

JNIEXPORT jlong JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_NodeGetParent(JNIEnv *env, jobject /* this */, jlong node) {
    if (0 == node) return 0;
    CMarkupNode* pNode = (CMarkupNode*)node;
    return (jlong)pNode->GetParent();
}

JNIEXPORT jlong JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_NodeGetSibling(JNIEnv *env, jobject /* this */, jlong node) {
    if (0 == node) return 0;
    CMarkupNode* pNode = (CMarkupNode*)node;
    return (jlong)pNode->GetSibling();
}

JNIEXPORT jlong JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_NodeGetChild(JNIEnv *env, jobject /* this */, jlong node) {
    if (0 == node) return 0;
    CMarkupNode* pNode = (CMarkupNode*)node;
    return (jlong)pNode->GetChild();
}

JNIEXPORT jlong JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_NodeGetChild2(JNIEnv *env, jobject /* this */, jlong node, jstring strName) {
    if (0 == node) return 0;
    CMarkupNode* pNode = (CMarkupNode*)node;
    char* pstrName = (char*)env->GetStringUTFChars(strName, 0);
    return (jlong)pNode->GetChild(pstrName);
}

JNIEXPORT jboolean JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_NodeIsValid(JNIEnv *env, jobject /* this */, jlong node) {
    if (0 == node) return 0;
    CMarkupNode* pNode = (CMarkupNode*)node;
    return pNode->IsValid();
}

JNIEXPORT jstring JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_NodeGetName(JNIEnv *env, jobject /* this */, jlong node) {
    if (0 == node) return 0;
    CMarkupNode* pNode = (CMarkupNode*)node;
    return env->NewStringUTF(pNode->GetName());
}

JNIEXPORT jboolean JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_NodeHasAttributes(JNIEnv *env, jobject /* this */, jlong node) {
    if (0 == node) return 0;
    CMarkupNode* pNode = (CMarkupNode*)node;
    return pNode->HasAttributes();
}

JNIEXPORT jstring JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_NodeGetAttributeValueWithName(JNIEnv *env, jobject /* this */, jlong node, jstring name) {
    if (0 == node) return 0;
    char* pname = (char*)env->GetStringUTFChars(name, 0);
    CMarkupNode* pNode = (CMarkupNode*)node;
    return env->NewStringUTF(pNode->GetAttributeValue((const char*)pname));
}

JNIEXPORT jboolean JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_NodeHasChildren(JNIEnv *env, jobject /* this */, jlong node) {
    if (0 == node) return 0;
    CMarkupNode* pNode = (CMarkupNode*)node;
    return pNode->HasChildren();
}

JNIEXPORT jint JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_NodeGetAttributeCount(JNIEnv *env, jobject /* this */, jlong node) {
    if (0 == node) return 0;
    CMarkupNode* pNode = (CMarkupNode*)node;
    return pNode->GetAttributeCount();
}

JNIEXPORT jstring JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_NodeGetAttributeName(JNIEnv *env, jobject /* this */, jlong node, jint index) {
    if (0 == node) return 0;
    CMarkupNode* pNode = (CMarkupNode*)node;
    return env->NewStringUTF(pNode->GetAttributeName(index));
}

JNIEXPORT jstring JNICALL Java_fyuilib_luoc_com_core_FyUILibJNI_NodeGetAttributeValueWithIndex(JNIEnv *env, jobject /* this */, jlong node, jint index) {
    if (0 == node) return 0;
    CMarkupNode* pNode = (CMarkupNode*)node;
    return env->NewStringUTF(pNode->GetAttributeValue(index));
}

}



