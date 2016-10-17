#ifndef __vdm_jnienv_h__
#define __vdm_jnienv_h__

#ifdef _MSC_VER
#include <windows.h>
#endif // _MSC_VER

#include "dlclass.h"
#include "jni.h"

class vdm_JFieldID : public DlClass {
public:
  vdm_JFieldID(jfieldID fid, const SEQ<Char> & nm, const SEQ<Char> & sign)
   : _fid(fid), fieldNm(nm), fieldTp(sign) {};
  virtual ~vdm_JFieldID() {};
  const wchar_t * GetTypeName() { return L"jfieldID"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
  static jfieldID GetPtr(const DLObject& obj) { return GetDLPtr(obj)->_fid; };
  static SEQ<Char> GetFieldNm(const DLObject& obj) { return GetDLPtr(obj)->fieldNm; };
  static SEQ<Char> GetFieldTp(const DLObject& obj) { return GetDLPtr(obj)->fieldTp; };
private:
  vdm_JFieldID() {};
  static vdm_JFieldID *GetDLPtr(const DLObject& obj) { return static_cast<vdm_JFieldID *>(obj.GetPtr()); };
  jfieldID _fid;
  SEQ<Char> fieldNm;
  SEQ<Char> fieldTp;
};

class vdm_JMethodID : public DlClass {
public:
  vdm_JMethodID(jmethodID mid, const SEQ<Char> & nm, const SEQ<Char> & dTp, const SEQ<Char> & rTp)
    : _mid(mid), methodNm(nm), domTp(dTp), resTp(rTp) {};
  virtual ~vdm_JMethodID() {};
  const wchar_t * GetTypeName() { return L"jmethodID"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
  static jmethodID GetPtr(const DLObject& obj) { return GetDLPtr(obj)->_mid; };
  static SEQ<Char> GetMethodNm(const DLObject& obj) { return GetDLPtr(obj)->methodNm; };
  static SEQ< SEQ<Char> > GetDomTp(const DLObject& obj) { return GetDLPtr(obj)->domTp; };
  static SEQ<Char> GetResTp(const DLObject& obj) { return GetDLPtr(obj)->resTp; };
private:
  vdm_JMethodID() {};
  static vdm_JMethodID *GetDLPtr(const DLObject& obj) { return static_cast<vdm_JMethodID *>(obj.GetPtr()); };
  jmethodID _mid;
  SEQ<Char> methodNm;
  SEQ< SEQ<Char> > domTp;
  SEQ<Char> resTp;
};

class vdm_JObject : public DlClass {
public:
  vdm_JObject(JNIEnv *e, jobject obj, const SEQ<Char> & sign);
  virtual ~vdm_JObject();
  const wchar_t * GetTypeName() { return L"jobject"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
  static jobject GetPtr(const DLObject& obj) { return GetDLPtr(obj)->_obj; };
  static SEQ<Char> GetTp(const DLObject& obj) { return GetDLPtr(obj)->classTp; };
protected:
  Generic toString();
  Generic equals(const Sequence & p);
  Generic getClass();
private:
  vdm_JObject() {};
  static vdm_JObject *GetDLPtr(const DLObject& obj) { return static_cast<vdm_JObject *>(obj.GetPtr()); };
protected:
  jobject _obj;
  JNIEnv *env;
  Sequence classTp;
};

class vdm_JClass : public vdm_JObject {
public:
  vdm_JClass(JNIEnv *e, jobject obj, const SEQ<Char> & sign) : vdm_JObject(e, obj, sign) {};
  virtual ~vdm_JClass() {};
  const wchar_t * GetTypeName() { return L"jclass"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
protected:
  Generic getName();
};

class vdm_JThrowable : public vdm_JObject {
public:
  vdm_JThrowable(JNIEnv *e, jobject obj, const SEQ<Char> & sign) : vdm_JObject(e, obj, sign) {};
  virtual ~vdm_JThrowable() {};
  const wchar_t * GetTypeName() { return L"jthrowable"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
protected:
  Generic getMessage();
  Generic getStackTrace();
};

class vdm_JString : public vdm_JObject {
public:
  vdm_JString(JNIEnv *e, jobject obj, const SEQ<Char> & sign) : vdm_JObject(e, obj, sign) {};
  virtual ~vdm_JString() {};
  const wchar_t * GetTypeName() { return L"jstring"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
};

class vdm_JArray : public vdm_JObject {
public:
  vdm_JArray(JNIEnv *e, jobject obj, const SEQ<Char> & sign) : vdm_JObject(e, obj, sign) {};
  virtual ~vdm_JArray() {};
  const wchar_t * GetTypeName() { return L"jarray"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
};

class vdm_JBooleanArray : public vdm_JArray {
public:
  vdm_JBooleanArray(JNIEnv *e, jobject obj, const SEQ<Char> & sign) : vdm_JArray(e, obj, sign) {};
  virtual ~vdm_JBooleanArray() {};
  const wchar_t * GetTypeName() { return L"jbooleanArray"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
};

class vdm_JByteArray : public vdm_JArray {
public:
  vdm_JByteArray(JNIEnv *e, jobject obj, const SEQ<Char> & sign) : vdm_JArray(e, obj, sign) {};
  virtual ~vdm_JByteArray() {};
  const wchar_t * GetTypeName() { return L"jbyteArray"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
};

class vdm_JCharArray : public vdm_JArray {
public:
  vdm_JCharArray(JNIEnv *e, jobject obj, const SEQ<Char> & sign) : vdm_JArray(e, obj, sign) {};
  virtual ~vdm_JCharArray() {};
  const wchar_t * GetTypeName() { return L"jcharArray"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
};

class vdm_JShortArray : public vdm_JArray {
public:
  vdm_JShortArray(JNIEnv *e, jobject obj, const SEQ<Char> & sign) : vdm_JArray(e, obj, sign) {};
  virtual ~vdm_JShortArray() {};
  const wchar_t * GetTypeName() { return L"jshortArray"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
};

class vdm_JIntArray : public vdm_JArray {
public:
  vdm_JIntArray(JNIEnv *e, jobject obj, const SEQ<Char> & sign) : vdm_JArray(e, obj, sign) {};
  virtual ~vdm_JIntArray() {};
  const wchar_t * GetTypeName() { return L"jintArray"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
};

class vdm_JLongArray : public vdm_JArray {
public:
  vdm_JLongArray(JNIEnv *e, jobject obj, const SEQ<Char> & sign) : vdm_JArray(e, obj, sign) {};
  virtual ~vdm_JLongArray() {};
  const wchar_t * GetTypeName() { return L"jlongArray"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
};

class vdm_JFloatArray : public vdm_JArray {
public:
  vdm_JFloatArray(JNIEnv *e, jobject obj, const SEQ<Char> & sign) : vdm_JArray(e, obj, sign) {};
  virtual ~vdm_JFloatArray() {};
  const wchar_t * GetTypeName() { return L"jfloatArray"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
};

class vdm_JDoubleArray : public vdm_JArray {
public:
  vdm_JDoubleArray(JNIEnv *e, jobject obj, const SEQ<Char> & sign) : vdm_JArray(e, obj, sign) {};
  virtual ~vdm_JDoubleArray() {};
  const wchar_t * GetTypeName() { return L"jdoubleArray"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
};

class vdm_JObjectArray : public vdm_JArray {
public:
  vdm_JObjectArray(JNIEnv *e, jobject obj, const SEQ<Char> & sign) : vdm_JArray(e, obj, sign) {};
  virtual ~vdm_JObjectArray() {};
  const wchar_t * GetTypeName() { return L"jobjectArray"; };
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);
};

class vdm_JNIEnv : public DlClass
{
public:
  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  vdm_JNIEnv ();
  virtual ~vdm_JNIEnv ();

private:
  Generic CreateJavaVM(const Sequence & p);
  Generic DestroyJavaVM();
  Generic GetEnv(const Sequence & p);
  Generic AttachCurrentThread();

  Generic GetVersion();
  Generic GetTp(const Sequence & p);
  Generic GetFieldName(const Sequence & p);
  Generic GetMethodName(const Sequence & p);
  Generic FindClass(const Sequence & p);
  Generic GetObjectClass(const Sequence & p);
  Generic IsInstanceOf(const Sequence & p);
  Generic GetStaticFieldID(const Sequence & p);
  Generic GetFieldID(const Sequence & p);
  Generic GetStaticField(const Sequence & p);
  Generic SetStaticField(const Sequence & p);
  Generic GetField(const Sequence & p);
  Generic SetField(const Sequence & p);
  Generic GetStaticMethodID(const Sequence & p);
  Generic GetMethodID(const Sequence & p);
  Generic CallStaticMethod(const Sequence & p);
  Generic NewObject(const Sequence & p);
  Generic CallMethod(const Sequence & p);
  Generic GetArrayLength(const Sequence & p);
  Generic GetArray(const Sequence & p);
  Generic SetArray(const Sequence & p);
  Generic NewArray(const Sequence & p);
  Generic NewString(const Sequence & p);
  Generic GetStringLength(const Sequence & p);
  Generic GetStringChars(const Sequence & p);

public:
  static void SetJValue(jvalue * jv, const Sequence & args, const SEQ< SEQ<Char> > & sign);
  static Generic MakeDLObject(JNIEnv *env, jobject o, const SEQ<Char> & restp);
  static Generic CallNoArgObjectMethod(JNIEnv *env, jobject obj,  const SEQ<Char> & mtnm, const SEQ<Char> & restp);

  static Tuple ConvSign(const Sequence & sign);
  static Sequence SignStringToSeq(const wstring & str);
private:
  JNIEnv *env;
  JavaVM *jvm;
  static bool jvmattached;
};
#endif // __vdm_jnienv_h__
