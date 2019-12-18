#include "jnienv.h"
#include "metaiv.h"
#include <vector>

extern string wstring2utf8str(const wstring & ws);
extern wstring utf8str2wstring(const string & s);

#define THROWABLE(envp, ex) DLObject(L"jthrowable", new vdm_JThrowable(envp, ex, SEQ<Char>(L"Ljava/lang/Throwable;")))

bool vdm_JNIEnv::jvmattached = false;

DlClass* DlClass_new (const wchar_t* name)
{
  if (!wcscmp (name, L"JNIEnv")) {
    return new vdm_JNIEnv ();
  }
  else {
    return NULL; // nothing found
  }
}

int DlClass_delete (DlClass* c)
{
  try {
    delete c;
  }
  catch (...) {
    return 0;
  }
  return 1;
}

Generic DlClass_call (DlClass* c, const wchar_t* name, const Sequence & params, int & success)
{
  Generic result;
  try {
    result = c->DlMethodCall (name, params);
    success = 1;
    return result;
  }
  catch (...) {
    success = 0;
    return result;
  }
}

vdm_JNIEnv::vdm_JNIEnv()
{
  this->env = NULL;
  this->jvm = NULL;
}

vdm_JNIEnv::~vdm_JNIEnv ()
{
  vdm_JNIEnv::jvmattached = false;
  this->env = NULL;
  this->jvm = NULL;
}

Generic vdm_JNIEnv::DlMethodCall (const wchar_t * name, const Sequence & p)
{
#ifdef _MSC_VER
  // Temporary Hack for segmentation violation
  wostringstream wos;
  wos << p << endl;
  // <-- hack end
#endif // _MSC_VER

//wcout << L"DlMethodCall: " << name << L" " << p << endl;
//wcout << L"jvmattached: " << (vdm_JNIEnv::jvmattached ? L"true" : L"false") << endl;
  Generic res;
  if (wcscmp(name, L"IsJVMAttached")==0) {
    return Bool(vdm_JNIEnv::jvmattached);
  }

  if (!vdm_JNIEnv::jvmattached) {
    if (wcscmp(name, L"CreateJavaVM")==0) {
      return CreateJavaVM(p);
    }
    else if (wcscmp(name, L"GetEnv")==0) {
      return GetEnv(p);
    }
    else if (wcscmp(name, L"AttachCurrentThread")==0) {
      return AttachCurrentThread();
    }
    return Generic();
  }
  else if (wcscmp(name, L"DestroyJavaVM")==0) {
    res = DestroyJavaVM();
  }
  else if (wcscmp(name, L"GetVersion")==0) {
    res = GetVersion();
  }
  else if (wcscmp(name, L"GetTp")==0) {
    res = GetTp(p);
  }
  else if (wcscmp(name, L"GetFieldName")==0) {
    res = GetFieldName(p);
  }
  else if (wcscmp(name, L"GetMethodName")==0) {
    res = GetFieldName(p);
  }
  else if (wcscmp(name, L"FindClass_i")==0) {
    res = FindClass(p);
  }
  else if (wcscmp(name, L"GetObjectClass_i")==0) {
    res = GetObjectClass(p);
  }
  else if (wcscmp(name, L"IsInstanceOf_i")==0) {
    res = IsInstanceOf(p);
  }
  else if (wcscmp(name, L"GetStaticFieldID_i")==0) {
    res = GetStaticFieldID(p);
  }
  else if (wcscmp(name, L"GetFieldID_i")==0) {
    res = GetFieldID(p);
  }
  else if (wcscmp(name, L"GetStaticField_i")==0) {
    res = GetStaticField(p);
  }
  else if (wcscmp(name, L"SetStaticField_i")==0) {
    res = SetStaticField(p);
  }
  else if (wcscmp(name, L"GetField_i")==0) {
    res = GetField(p);
  }
  else if (wcscmp(name, L"SetField_i")==0) {
    res = SetField(p);
  }
  else if (wcscmp(name, L"GetStaticMethodID_i")==0) {
    res = GetStaticMethodID(p);
  }
  else if (wcscmp(name, L"GetMethodID_i")==0) {
    res = GetMethodID(p);
  }
  else if (wcscmp(name, L"CallStaticMethod_i")==0) {
    res = CallStaticMethod(p);
  }
  else if (wcscmp(name, L"NewObject_i")==0) {
    res = NewObject(p);
  }
  else if (wcscmp(name, L"CallMethod_i")==0) {
    res = CallMethod(p);
  }
  else if (wcscmp(name, L"GetArrayLength_i")==0) {
    res = GetArrayLength(p);
  }
  else if (wcscmp(name, L"GetArray_i")==0) {
    res = GetArray(p);
  }
  else if (wcscmp(name, L"SetArray_i")==0) {
    res = SetArray(p);
  }
  else if (wcscmp(name, L"NewArray_i")==0) {
    res = NewArray(p);
  }
  else if (wcscmp(name, L"NewString_i")==0) {
    res = NewString(p);
  }
  else if (wcscmp(name, L"GetStringLength_i")==0) {
    res = GetStringLength(p);
  }
  else if (wcscmp(name, L"GetStringChars_i")==0) {
    res = GetStringChars(p);
  }
  else {
  }
  return res;
}

// CreateJavaVM: jint * seq of seq of char ==> jint
Generic vdm_JNIEnv::CreateJavaVM(const Sequence & p)
{
  JavaVMInitArgs vm_args;
  vm_args.version = Real(p[1]).GetIntValue();

  SEQ< SEQ<Char> > optionSeq (p[2]);
  int len_optionSeq = optionSeq.Length();
  JavaVMOption * options = new JavaVMOption[len_optionSeq];
  vector<string> ops;
  for (int i = 1; i <= len_optionSeq; i++) {
    ops.push_back(wstring2string(optionSeq[i].GetString()));
    options[i-1].optionString = (char *)((*ops.rbegin()).c_str());
  }
  vm_args.options = options;
  vm_args.nOptions = len_optionSeq;
  vm_args.ignoreUnrecognized = true;

  /* Java VM の作成 */
  jint res = JNI_CreateJavaVM(&this->jvm, (void **)&this->env, &vm_args);
  delete [] options;
  if (res == JNI_OK) {
    vdm_JNIEnv::jvmattached = true;
  } 
  return Int(res);
}

// DestroyJavaVM: () ==> jint
Generic vdm_JNIEnv::DestroyJavaVM()
{
  /* Java VM の破棄 */
  jint res = this->jvm->DestroyJavaVM();
  this->jvm = NULL;
  this->env = NULL;
  return Int(res);
}

// GetEnv: jint ==> jint
Generic vdm_JNIEnv::GetEnv(const Sequence & p)
{
  if (this->jvm == NULL) {
    jsize ct;
    jint res = JNI_GetCreatedJavaVMs(&this->jvm, 1, &ct);
    if (res != JNI_OK) {
      return Int(res);
    }
  }
  return Int(this->jvm->GetEnv((void**)&this->env, Real(p[1]).GetIntValue()));
}

// AttachCurrentThread: () ==> jint
Generic vdm_JNIEnv::AttachCurrentThread()
{
  jint res = this->jvm->AttachCurrentThread((void**)&this->env, NULL);
  if (res == JNI_OK) {
    vdm_JNIEnv::jvmattached = true;
  }
  return Int(res);
}

// GetVersion: () ==> jint
Generic vdm_JNIEnv::GetVersion()
{
  if (vdm_JNIEnv::jvmattached) {
    return Int(this->env->GetVersion());
  }
  else {
    return Int(-1);
  }
}

// GetTp: jobject ==> seq of char
Generic vdm_JNIEnv::GetTp(const Sequence & p)
{
  return vdm_JObject::GetTp(static_cast<DLObject>(p[1]));
}

// GetFieldName: jfieldID ==> seq of char
Generic vdm_JNIEnv::GetFieldName(const Sequence & p)
{
  return vdm_JFieldID::GetFieldNm(static_cast<DLObject>(p[1]));
}

// GetMethodName: jmethodID ==> seq of char
Generic vdm_JNIEnv::GetMethodName(const Sequence & p)
{
  return vdm_JMethodID::GetMethodNm(static_cast<DLObject>(p[1]));
}

// FindClass: seq of char ==> [jclass]
Generic vdm_JNIEnv::FindClass(const Sequence & p)
{
  string clnm (wstring2utf8str(Sequence(p[1]).GetString())); 

  this->env->ExceptionClear();
  jclass cls = this->env->FindClass(clnm.c_str());
  jthrowable ex = this->env->ExceptionOccurred();

  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    return mk_(DLObject(L"jclass", new vdm_JClass(this->env, cls, p[1])), Nil());
  }
}

// GetObjectClass: jobject ==> [jclass] * [jthrowable]
Generic vdm_JNIEnv::GetObjectClass(const Sequence & p)
{
  jobject obj = vdm_JObject::GetPtr(static_cast<DLObject>(p[1]));
  SEQ<Char> tp (vdm_JObject::GetTp(static_cast<DLObject>(p[1])));

  this->env->ExceptionClear();
  jclass cls = this->env->GetObjectClass(obj);
  jthrowable ex = this->env->ExceptionOccurred();

  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    return mk_(DLObject(L"jclass", new vdm_JClass(this->env, cls, tp)), Nil());
  }
}

// IsInstanceOf: jobject * jclass ==> [jboolean] * [jthrowable]
Generic vdm_JNIEnv::IsInstanceOf(const Sequence & p)
{
  jobject obj = vdm_JObject::GetPtr(static_cast<DLObject>(p[1]));
  jclass cls = static_cast<jclass>(vdm_JObject::GetPtr(static_cast<DLObject>(p[2])));

  this->env->ExceptionClear();
  jboolean val = this->env->IsInstanceOf(obj, cls);
  jthrowable ex = this->env->ExceptionOccurred();

  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    return mk_(Bool(val), Nil());
  }
}

// GetStaticFieldID: jclass * seq of char * seq of char ==> [jfieldID] * [jthrowable]
Generic vdm_JNIEnv::GetStaticFieldID(const Sequence & p)
{
  jclass cls = static_cast<jclass>(vdm_JObject::GetPtr(static_cast<DLObject>(p[1])));
  string name (wstring2utf8str((Sequence(p[2]).GetString())));
  string sign (wstring2utf8str((Sequence(p[3]).GetString())));

  this->env->ExceptionClear();
  jfieldID fid = this->env->GetStaticFieldID(cls, name.c_str(), sign.c_str());
  jthrowable ex = this->env->ExceptionOccurred();

  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    return mk_(DLObject(L"jfieldID", new vdm_JFieldID(fid, p[2], p[3])), Nil());
  }
}

// GetFieldID: jclass * seq of char * seq of char ==> [jfieldID] * [jthrowable]
Generic vdm_JNIEnv::GetFieldID(const Sequence & p)
{
//wcout << L"GetFieldID: " << p << endl;
  jclass cls = static_cast<jclass>(vdm_JObject::GetPtr(static_cast<DLObject>(p[1])));
  string name (wstring2utf8str((Sequence(p[2]).GetString())));
  string sign (wstring2utf8str((Sequence(p[3]).GetString())));

  this->env->ExceptionClear();
  jfieldID fid = this->env->GetFieldID(cls, name.c_str(), sign.c_str());
  jthrowable ex = this->env->ExceptionOccurred();

  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    return mk_(DLObject(L"jfieldID", new vdm_JFieldID(fid, p[2], p[3])), Nil());
  }
}

// GetStaticField: jclass * jfieldID ==> [jvalue] * [jthrowable]
Generic vdm_JNIEnv::GetStaticField(const Sequence & p)
{
  jclass cls = static_cast<jclass>(vdm_JObject::GetPtr(static_cast<DLObject>(p[1])));
  jfieldID fid = vdm_JFieldID::GetPtr(static_cast<DLObject>(p[2]));
  wstring fldtp (vdm_JFieldID::GetFieldTp(static_cast<DLObject>(p[2])).GetString());

  this->env->ExceptionClear();
  if (fldtp == L"Z") {
    jboolean val = this->env->GetStaticBooleanField(cls, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Bool(val), Nil());
    }
  }
  else if (fldtp == L"B") {
    jbyte val = this->env->GetStaticByteField(cls, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Int(val), Nil());
    }
  }
  else if (fldtp == L"C") {
    jchar val = this->env->GetStaticCharField(cls, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Char(val), Nil());
    }
  }
  else if (fldtp == L"S") {
    jshort val = this->env->GetStaticShortField(cls, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Int(val), Nil());
    }
  }
  else if (fldtp == L"I") {
    jint val = this->env->GetStaticIntField(cls, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Int(val), Nil());
    }
  }
  else if (fldtp == L"J") {
    jlong val = this->env->GetStaticLongField(cls, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Int(val), Nil());
    }
  }
  else if (fldtp == L"F") {
    jfloat val = this->env->GetStaticFloatField(cls, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Real(val), Nil());
    }
  }
  else if (fldtp == L"D") {
    jdouble val = this->env->GetStaticDoubleField(cls, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Real(val), Nil());
    }
  }
  else {
    jobject resobj = this->env->GetStaticObjectField(cls, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else if (resobj != NULL) {
      return mk_(DLObject(L"jobject", new vdm_JObject(this->env, resobj, SEQ<Char>(fldtp))), Nil());
    }
    else {
      return mk_(Nil(), Nil());
    }
  }
}

// SetStaticField: jclass * jfieldID * jvalue ==> [jthrowable]
Generic vdm_JNIEnv::SetStaticField(const Sequence & p)
{
  jclass cls = static_cast<jclass>(vdm_JObject::GetPtr(static_cast<DLObject>(p[1])));
  jfieldID fid = vdm_JFieldID::GetPtr(static_cast<DLObject>(p[2]));
  wstring fldtp (vdm_JFieldID::GetFieldTp(static_cast<DLObject>(p[2])).GetString());

  this->env->ExceptionClear();
  if (fldtp == L"Z") {
    this->env->SetStaticBooleanField(cls, fid, Bool(p[3]).GetValue());
  }
  else if (fldtp == L"B") {
    this->env->SetStaticByteField(cls, fid, Int(p[3]).GetValue());
  }
  else if (fldtp == L"C") {
    this->env->SetStaticCharField(cls, fid, Char(p[3]).GetValue());
  }
  else if (fldtp == L"S") {
    this->env->SetStaticShortField(cls, fid, Int(p[3]).GetValue());
  }
  else if (fldtp == L"I") {
    this->env->SetStaticIntField(cls, fid, Int(p[3]).GetValue());
  }
  else if (fldtp == L"J") {
    this->env->SetStaticLongField(cls, fid, Int(p[3]).GetValue());
  }
  else if (fldtp == L"F") {
    this->env->SetStaticFloatField(cls, fid, Real(p[3]).GetValue());
  }
  else if (fldtp == L"D") {
    this->env->SetStaticDoubleField(cls, fid, Real(p[3]).GetValue());
  }
  else {
    this->env->SetStaticObjectField(cls, fid, vdm_JObject::GetPtr(static_cast<DLObject>(p[3])));
  }

  jthrowable ex = this->env->ExceptionOccurred();
  if (ex != NULL) {
    return THROWABLE(this->env, ex);
  }
  else {
    return Nil();
  }
}

// GetField: jobject * jfieldID ==> [jvalue] * [throwable]
Generic vdm_JNIEnv::GetField(const Sequence & p)
{
  jobject obj = static_cast<jobject>(vdm_JObject::GetPtr(static_cast<DLObject>(p[1])));
  jfieldID fid = vdm_JFieldID::GetPtr(static_cast<DLObject>(p[2]));
  wstring fldtp (vdm_JFieldID::GetFieldTp(static_cast<DLObject>(p[2])).GetString());

  this->env->ExceptionClear();
  if (fldtp == L"Z") {
    jboolean val = this->env->GetBooleanField(obj, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Bool(val), Nil());
    }
  }
  else if (fldtp == L"B") {
    jbyte val = this->env->GetByteField(obj, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Int(val), Nil());
    }
  }
  else if (fldtp == L"C") {
    jchar val = this->env->GetCharField(obj, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Char(val), Nil());
    }
  }
  else if (fldtp == L"S") {
    jshort val = this->env->GetShortField(obj, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Int(val), Nil());
    }
  }
  else if (fldtp == L"I") {
    jint val = this->env->GetIntField(obj, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Int(val), Nil());
    }
  }
  else if (fldtp == L"J") {
    jlong val = this->env->GetLongField(obj, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Int(val), Nil());
    }
  }
  else if (fldtp == L"F") {
    jfloat val = this->env->GetFloatField(obj, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Real(val), Nil());
    }
  }
  else if (fldtp == L"D") {
    jdouble val = this->env->GetDoubleField(obj, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else {
      return mk_(Real(val), Nil());
    }
  }
  else {
    jobject resobj = this->env->GetObjectField(obj, fid);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex));
    }
    else if (resobj != NULL) {
      return mk_(DLObject(L"jobject", new vdm_JObject(this->env, resobj, SEQ<Char>(fldtp))), Nil());
    }
    else {
      return mk_(Nil(), Nil());
    }
  }
}

// SetField: jobject * jfieldID * jvalue ==> [throwable]
Generic vdm_JNIEnv::SetField(const Sequence & p)
{
//wcout << L"SetField: " << p << endl;
  jobject obj = static_cast<jobject>(vdm_JObject::GetPtr(static_cast<DLObject>(p[1])));
  jfieldID fid = vdm_JFieldID::GetPtr(static_cast<DLObject>(p[2]));
  wstring fldtp (vdm_JFieldID::GetFieldTp(static_cast<DLObject>(p[2])).GetString());

  this->env->ExceptionClear();
  if (fldtp == L"Z") {
    this->env->SetBooleanField(obj, fid, Bool(p[3]).GetValue());
  }
  else if (fldtp == L"B") {
    this->env->SetByteField(obj, fid, Int(p[3]).GetValue());
  }
  else if (fldtp == L"C") {
    this->env->SetCharField(obj, fid, Char(p[3]).GetValue());
  }
  else if (fldtp == L"S") {
    this->env->SetShortField(obj, fid, Int(p[3]).GetValue());
  }
  else if (fldtp == L"I") {
    this->env->SetIntField(obj, fid, Int(p[3]).GetValue());
  }
  else if (fldtp == L"J") {
    this->env->SetLongField(obj, fid, Int(p[3]).GetValue());
  }
  else if (fldtp == L"F") {
    this->env->SetFloatField(obj, fid, Real(p[3]).GetValue());
  }
  else if (fldtp == L"D") {
    this->env->SetDoubleField(obj, fid, Real(p[3]).GetValue());
  }
  else {
    jobject o = vdm_JObject::GetPtr(static_cast<DLObject>(p[3]));
    this->env->SetObjectField(obj, fid, o);
  }
  jthrowable ex = this->env->ExceptionOccurred();
  if (ex != NULL) {
    return THROWABLE(this->env, ex);
  }
  else {
    return Nil();
  }
}

// GetStaticMethodID: jclass * seq of char * seq of char ==> [jmethodID] * [jthrowable]
Generic vdm_JNIEnv::GetStaticMethodID(const Sequence & p)
{
  jclass cls = static_cast<jclass>(vdm_JObject::GetPtr(static_cast<DLObject>(p[1])));
  string name (wstring2utf8str((Sequence(p[2]).GetString())));
  string sign (wstring2utf8str((Sequence(p[3]).GetString())));

  this->env->ExceptionClear();
  jmethodID mid = this->env->GetStaticMethodID(cls, name.c_str(), sign.c_str());
  jthrowable ex = this->env->ExceptionOccurred();

  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    Tuple t (ConvSign(p[3]));
    return mk_(DLObject(L"jmethodID", new vdm_JMethodID(mid, p[2], t.GetSequence(2), t.GetSequence(3))), Nil());
  }
}

// GetMethodID: jobject * seq of char * seq of char ==> [jmethodID] * [jthrowable]
Generic vdm_JNIEnv::GetMethodID(const Sequence & p)
{
//wcout << L"GetMethodID: " << p << endl;
  jclass cls = static_cast<jclass>(vdm_JObject::GetPtr(static_cast<DLObject>(p[1])));
  string name (wstring2utf8str((Sequence(p[2]).GetString())));
  string sign (wstring2utf8str((Sequence(p[3]).GetString())));

  this->env->ExceptionClear();
  jmethodID mid = this->env->GetMethodID(cls, name.c_str(), sign.c_str());
  jthrowable ex = this->env->ExceptionOccurred();

  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    Tuple t (ConvSign(p[3]));
    return mk_(DLObject(L"jmethodID", new vdm_JMethodID(mid, p[2], t.GetSequence(2), t.GetSequence(3))), Nil());
  }
}

// CallStaticMethod: jclass * jmethodID * seq of jvalue ==> [jvalue] * [jthrowable]
Generic vdm_JNIEnv::CallStaticMethod(const Sequence & p)
{
//wcout << L"CallStaticMethod: " << p << endl;
  jclass cls = static_cast<jclass>(vdm_JObject::GetPtr(static_cast<DLObject>(p[1])));
  jmethodID mid = vdm_JMethodID::GetPtr(static_cast<DLObject>(p[2]));
  Sequence args (p[3]);
  SEQ< SEQ<Char> > domtp (vdm_JMethodID::GetDomTp(static_cast<DLObject>(p[2])));
  wstring restp (vdm_JMethodID::GetResTp(static_cast<DLObject>(p[2])).GetString());

  jvalue * jv = new jvalue[args.Length()];
  SetJValue(jv, args, domtp);    
  this->env->ExceptionClear();
  Generic res = Nil();
  jthrowable ex = NULL;
  if (restp == L"V") { // void
    this->env->CallStaticVoidMethodA(cls, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Nil();
    }
  }
  else if (restp == L"Z") {
    jboolean val = this->env->CallStaticBooleanMethodA(cls, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Bool(val);
    }
  }
  else if (restp == L"B") {
    jbyte val = this->env->CallStaticByteMethodA(cls, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Int(val);
    }
  }
  else if (restp == L"C") {
    jchar val = this->env->CallStaticCharMethodA(cls, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Char(val);
    }
  }
  else if (restp == L"S") {
    jshort val = this->env->CallStaticShortMethodA(cls, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Int(val);
    }
  }
  else if (restp == L"I") {
    jint val = this->env->CallStaticIntMethodA(cls, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Int(val);
    }
  }
  else if (restp == L"J") {
    jlong val = this->env->CallStaticLongMethodA(cls, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Int(val);
    }
  }
  else if (restp == L"F") {
    jfloat val = this->env->CallStaticFloatMethodA(cls, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Real(val);
    }
  }
  else if (restp == L"D") {
    jdouble val = this->env->CallStaticDoubleMethodA(cls, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Real(val);
    }
  }
  else {
    jobject resobj = this->env->CallStaticObjectMethodA(cls, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = MakeDLObject(this->env, resobj, SEQ<Char>(restp));
    }
  }
  delete [] jv;

  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    return mk_(res, Nil());
  }
}

// NewObject: jclass * jmethodID * seq of jvalue ==> [jobject] * [jthrowable]
Generic vdm_JNIEnv::NewObject(const Sequence & p)
{
//wcout << L"NewObject: " << p << endl;
  jclass cls = static_cast<jclass>(vdm_JObject::GetPtr(static_cast<DLObject>(p[1])));
  SEQ<Char> tp (vdm_JObject::GetTp(static_cast<DLObject>(p[1])));
  jmethodID mid = vdm_JMethodID::GetPtr(static_cast<DLObject>(p[2]));
  Sequence args (p[3]);
  SEQ< SEQ<Char> > domtp (vdm_JMethodID::GetDomTp(static_cast<DLObject>(p[2])));

  jvalue *jv = new jvalue[args.Length()];
  SetJValue(jv, args, domtp);    

  this->env->ExceptionClear();
  jobject obj = this->env->NewObjectA(cls, mid, jv);
  jthrowable ex = this->env->ExceptionOccurred();

  delete [] jv;

  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    return mk_(DLObject(L"jobject", new vdm_JObject(this->env, obj, tp)), Nil());
  }
}

// CallMethod: jobject * jmethodID * seq of jvalue ==> [jvalue] * [jthrowable]
Generic vdm_JNIEnv::CallMethod(const Sequence & p)
{
  jobject obj = vdm_JObject::GetPtr(static_cast<DLObject>(p[1]));
  jmethodID mid = vdm_JMethodID::GetPtr(static_cast<DLObject>(p[2]));
  Sequence args (p[3]);
  SEQ< SEQ<Char> > domtp (vdm_JMethodID::GetDomTp(static_cast<DLObject>(p[2])));
  wstring restp (vdm_JMethodID::GetResTp(static_cast<DLObject>(p[2])).GetString());

  jvalue *jv = new jvalue[args.Length()];
  SetJValue(jv, args, domtp);    
  this->env->ExceptionClear();

  jthrowable ex = NULL;
  Generic res = Nil();
  if (restp == L"V") { // void
    this->env->CallVoidMethodA(obj, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Nil();
    }
  }
  else if (restp == L"Z") {
    jboolean val = this->env->CallBooleanMethodA(obj, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Bool(val);
    }
  }
  else if (restp == L"B") {
    jbyte val = this->env->CallByteMethodA(obj, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Int(val);
    }
  }
  else if (restp == L"C") {
    jchar val = this->env->CallCharMethodA(obj, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Char(val);
    }
  }
  else if (restp == L"S") {
    jshort val = this->env->CallShortMethodA(obj, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Int(val);
    }
  }
  else if (restp == L"I") {
    jint val = this->env->CallIntMethodA(obj, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Int(val);
    }
  }
  else if (restp == L"J") {
    jlong val = this->env->CallLongMethodA(obj, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Int(val);
    }
  }
  else if (restp == L"F") {
    jfloat val = this->env->CallFloatMethodA(obj, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Real(val);
    }
  }
  else if (restp == L"D") {
    jdouble val = this->env->CallDoubleMethodA(obj, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = Real(val);
    }
  }
  else {
    jobject resobj = this->env->CallObjectMethodA(obj, mid, jv);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      res = MakeDLObject(this->env, resobj, SEQ<Char>(restp));
    }
  }
  delete [] jv;

  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    return mk_(res, Nil());
  }
}

// GetArrayLength: jarray ==> [jsize] * [jthrowable]
Generic vdm_JNIEnv::GetArrayLength(const Sequence & p)
{
  jarray array = static_cast<jarray>(vdm_JObject::GetPtr(static_cast<DLObject>(p[1])));

  this->env->ExceptionClear();
  jint size = this->env->GetArrayLength(array);
  jthrowable ex = this->env->ExceptionOccurred();

  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    return mk_(Int(size), Nil());
  }
}

// GetArray: jarray ==> [seq of jvalue] * [jthrowable]
Generic vdm_JNIEnv::GetArray(const Sequence & p)
{
  jarray array = static_cast<jarray>(vdm_JObject::GetPtr(static_cast<DLObject>(p[1])));
  jint len = this->env->GetArrayLength(array);
  wstring arrayTp (vdm_JObject::GetTp(static_cast<DLObject>(p[1])).GetString());
  jthrowable ex = NULL;
  Generic res = Nil();
  this->env->ExceptionClear();
  if (arrayTp == L"[Z") {
    jboolean * buf = new jboolean[len];
    this->env->GetBooleanArrayRegion(static_cast<jbooleanArray>(array), 0, len, buf);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      Sequence val;
      for (size_t i = 0; i < len; i++) {
        val.ImpAppend(Bool(buf[i]));
      }
      res = val;
    }
    delete [] buf;
  }
  else if (arrayTp == L"[B") {
    jbyte * buf = new jbyte[len];
    this->env->GetByteArrayRegion(static_cast<jbyteArray>(array), 0, len, buf);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      Sequence val;
      for (size_t i = 0; i < len; i++) {
        val.ImpAppend(Int(buf[i]));
     }
      res = val;
    }
    delete [] buf;
  }
  else if (arrayTp == L"[C") {
    jchar * buf = new jchar[len];
    this->env->GetCharArrayRegion(static_cast<jcharArray>(array), 0, len, buf);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      Sequence val;
      for (size_t i = 0; i < len; i++) {
        val.ImpAppend(Char(buf[i]));
      }
      res = val;
    }
    delete [] buf;
  }
  else if (arrayTp == L"[S") {
    jshort * buf = new jshort[len];
    this->env->GetShortArrayRegion(static_cast<jshortArray>(array), 0, len, buf);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      Sequence val;
      for (size_t i = 0; i < len; i++) {
        val.ImpAppend(Int(buf[i]));
      }
      res = val;
    }
    delete [] buf;
  }
  else if (arrayTp == L"[I") {
    jint * buf = new jint[len];
    this->env->GetIntArrayRegion(static_cast<jintArray>(array), 0, len, buf);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      Sequence val;
      for (size_t i = 0; i < len; i++) {
        val.ImpAppend(Int(buf[i]));
      }
      res = val;
    }
    delete [] buf;
  }
  else if (arrayTp == L"[J") {
    jlong * buf = new jlong[len];
    this->env->GetLongArrayRegion(static_cast<jlongArray>(array), 0, len, buf);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      Sequence val;
      for (size_t i = 0; i < len; i++) {
        val.ImpAppend(Int(buf[i]));
      }
      res = val;
    }
    delete [] buf;
  }
  else if (arrayTp == L"[F") {
    jfloat * buf = new jfloat[len];
    this->env->GetFloatArrayRegion(static_cast<jfloatArray>(array), 0, len, buf);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      Sequence val;
      for (size_t i = 0; i < len; i++) {
        val.ImpAppend(Real(buf[i]));
      }
      res = val;
    }
    delete [] buf;
  }
  else if (arrayTp == L"[D") {
    jdouble * buf = new jdouble[len];
    this->env->GetDoubleArrayRegion(static_cast<jdoubleArray>(array), 0, len, buf);
    ex = this->env->ExceptionOccurred();
    if (ex == NULL) {
      Sequence val;
      for (size_t i = 0; i < len; i++) {
        val.ImpAppend(Real(buf[i]));
      }
      res = val;
    }
    delete [] buf;
  }
  else {
    SEQ<Char> innerTp (arrayTp.substr(1));
    Sequence val;
    for (size_t i = 0; i < len; i++) {
      jobject obj = this->env->GetObjectArrayElement(static_cast<jobjectArray>(array), i);
      ex = this->env->ExceptionOccurred();
      if (ex != NULL) {
        return mk_(Nil(), THROWABLE(this->env, ex));
      }
      else {
        val.ImpAppend(DLObject(L"jobject", new vdm_JObject(this->env, obj, innerTp)));
      }
    }
    res = val;
  }
  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    return mk_(res, Nil());
  }
}

// SetArray: jarray * seq of jvalue ==> [jthrowable]
Generic vdm_JNIEnv::SetArray(const Sequence & p)
{
//wcout << L"SetArray: " << p << endl;
  jarray array = static_cast<jarray>(vdm_JObject::GetPtr(static_cast<DLObject>(p[1])));
  Sequence val (p[2]);
  jint len = val.Length();
  wstring arrayTp (vdm_JObject::GetTp(static_cast<DLObject>(p[1])).GetString());
  this->env->ExceptionClear();
  if (arrayTp == L"[Z") {
    jboolean * buf = new jboolean[len];
    for (size_t i = 0; i < len; i++) {
      buf[i] = Bool(val[i+1]).GetValue();
    }
    this->env->SetBooleanArrayRegion(static_cast<jbooleanArray>(array), 0, len, buf);
    jthrowable ex = this->env->ExceptionOccurred();
    delete [] buf;
    if (ex != NULL) {
      return THROWABLE(this->env, ex);
    }
  }
  else if (arrayTp == L"[B") {
    jbyte * buf = new jbyte[len];
    for (size_t i = 0; i < len; i++) {
      buf[i] = Int(val[i+1]).GetValue();
    }
    this->env->SetByteArrayRegion(static_cast<jbyteArray>(array), 0, len, buf);
    jthrowable ex = this->env->ExceptionOccurred();
    delete [] buf;
    if (ex != NULL) {
      return THROWABLE(this->env, ex);
    }
  }
  else if (arrayTp == L"[C") {
    jchar * buf = new jchar[len];
    for (size_t i = 0; i < len; i++) {
      buf[i] = Char(val[i+1]).GetValue();
    }
    this->env->SetCharArrayRegion(static_cast<jcharArray>(array), 0, len, buf);
    jthrowable ex = this->env->ExceptionOccurred();
    delete [] buf;
    if (ex != NULL) {
      return THROWABLE(this->env, ex);
    }
  }
  else if (arrayTp == L"[S") {
    jshort * buf = new jshort[len];
    for (size_t i = 0; i < len; i++) {
      buf[i] = Int(val[i+1]).GetValue();
    }
    this->env->SetShortArrayRegion(static_cast<jshortArray>(array), 0, len, buf);
    jthrowable ex = this->env->ExceptionOccurred();
    delete [] buf;
    if (ex != NULL) {
      return THROWABLE(this->env, ex);
    }
  }
  else if (arrayTp == L"[I") {
    jint * buf = new jint[len];
    for (size_t i = 0; i < len; i++) {
      buf[i] = Int(val[i+1]).GetValue();
    }
    this->env->SetIntArrayRegion(static_cast<jintArray>(array), 0, len, buf);
    jthrowable ex = this->env->ExceptionOccurred();
    delete [] buf;
    if (ex != NULL) {
      return THROWABLE(this->env, ex);
    }
  }
  else if (arrayTp == L"[J") {
    jlong * buf = new jlong[len];
    for (size_t i = 0; i < len; i++) {
      buf[i] = Int(val[i+1]).GetValue();
    }
    this->env->SetLongArrayRegion(static_cast<jlongArray>(array), 0, len, buf);
    jthrowable ex = this->env->ExceptionOccurred();
    delete [] buf;
    if (ex != NULL) {
      return THROWABLE(this->env, ex);
    }
  }
  else if (arrayTp == L"[F") {
    jfloat * buf = new jfloat[len];
    for (size_t i = 0; i < len; i++) {
      buf[i] = Real(val[i+1]).GetValue();
    }
    this->env->SetFloatArrayRegion(static_cast<jfloatArray>(array), 0, len, buf);
    jthrowable ex = this->env->ExceptionOccurred();
    delete [] buf;
    if (ex != NULL) {
      return THROWABLE(this->env, ex);
    }
  }
  else if (arrayTp == L"[D") {
    jdouble * buf = new jdouble[len];
    for (size_t i = 0; i < len; i++) {
      buf[i] = Real(val[i+1]).GetValue();
    }
    this->env->SetDoubleArrayRegion(static_cast<jdoubleArray>(array), 0, len, buf);
    jthrowable ex = this->env->ExceptionOccurred();
    delete [] buf;
    if (ex != NULL) {
      return THROWABLE(this->env, ex);
    }
  }
  else {
    for (size_t i = 0; i < len; i++) {
      jobject obj = vdm_JObject::GetPtr(static_cast<DLObject>(val[i+1]));
      this->env->SetObjectArrayElement(static_cast<jobjectArray>(array), i, obj);
      jthrowable ex = this->env->ExceptionOccurred();
      if (ex != NULL) {
        return THROWABLE(this->env, ex);
      }
    }
  }
  return Nil();
}

// NewArray: jsize * seq of char ==> [jarray] * [jthrowable]
Generic vdm_JNIEnv::NewArray(const Sequence & p)
{
//wcout << L"NewArray: " << p << endl;
  jsize len = Real(p[1]).GetIntValue();
  wstring arrayTp (Sequence(p[2]).GetString());
  this->env->ExceptionClear();
  if (arrayTp == L"[Z") {
    jbooleanArray obj = this->env->NewBooleanArray(len);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex)); 
    }
    else {
      return mk_(DLObject(L"jbooleanArray", new vdm_JBooleanArray(this->env, obj, p[2])), Nil());
    }
  }
  else if (arrayTp == L"[B") {
    jbyteArray obj = this->env->NewByteArray(len);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex)); 
    }
    else {
      return mk_(DLObject(L"jbyteArray", new vdm_JByteArray(this->env, obj, p[2])), Nil());
    }
  }
  else if (arrayTp == L"[C") {
    jcharArray obj = this->env->NewCharArray(len);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex)); 
    }
    else {
      return mk_(DLObject(L"jcharArray", new vdm_JCharArray(this->env, obj, p[2])), Nil());
    }
  }
  else if (arrayTp == L"[S") {
    jshortArray obj = this->env->NewShortArray(len);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex)); 
    }
    else {
      return mk_(DLObject(L"jshortArray", new vdm_JShortArray(this->env, obj, p[2])), Nil());
    }
  }
  else if (arrayTp == L"[I") {
    jintArray obj = this->env->NewIntArray(len);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex)); 
    }
    else {
      return mk_(DLObject(L"jintArray", new vdm_JIntArray(this->env, obj, p[2])), Nil());
    }
  }
  else if (arrayTp == L"[J") {
    jlongArray obj = this->env->NewLongArray(len);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex)); 
    }
    else {
      return mk_(DLObject(L"jlongArray", new vdm_JLongArray(this->env, obj, p[2])), Nil());
    }
  }
  else if (arrayTp == L"[F") {
    jfloatArray obj = this->env->NewFloatArray(len);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex)); 
    }
    else {
      return mk_(DLObject(L"jfloatArray", new vdm_JFloatArray(this->env, obj, p[2])), Nil());
    }
  }
  else if (arrayTp == L"[D") {
    jdoubleArray obj = this->env->NewDoubleArray(len);
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex)); 
    }
    else {
      return mk_(DLObject(L"jdoubleArray", new vdm_JDoubleArray(this->env, obj, p[2])), Nil());
    }
  }
  else {
    string clnm (wstring2utf8str(arrayTp).substr(1));
    jclass cls = this->env->FindClass(clnm.c_str());
    jthrowable ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex)); 
    }
    jobjectArray obj = this->env->NewObjectArray(len, cls, NULL);
    ex = this->env->ExceptionOccurred();
    if (ex != NULL) {
      return mk_(Nil(), THROWABLE(this->env, ex)); 
    }
    else {
      return mk_(DLObject(L"jobjectArray", new vdm_JObjectArray(this->env, obj, p[2])), Nil());
    }
  }
}

// NewString: seq of char ==> [jstring] * [jthrowable]
Generic vdm_JNIEnv::NewString(const Sequence & p)
{
  SEQ<Char> str (p[1]); 
  jsize len = str.Length();
  jchar* buf = new jchar[len];
  for (size_t i = 0; i < len; i++) {
    buf[i] = str[i+1].GetValue();
  }
  this->env->ExceptionClear();
  jstring obj = this->env->NewString(buf, len);
  jthrowable ex = this->env->ExceptionOccurred();

  delete [] buf;

  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    return mk_(DLObject(L"jstring", new vdm_JString(this->env, obj, SEQ<Char>(L"Ljava/lang/String"))), Nil());
  }
}

// GetStringLength: jstring ==> [jsize] * [jthrowable]
Generic vdm_JNIEnv::GetStringLength(const Sequence & p)
{
  jstring js = static_cast<jstring>(vdm_JObject::GetPtr(static_cast<DLObject>(p[1])));

  this->env->ExceptionClear();
  jsize size = this->env->GetStringLength(js);
  jthrowable ex = this->env->ExceptionOccurred();

  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    return mk_(Int(size), Nil());
  }
}

// GetStringChars: jstring ==> [seq of char] * [jthrowable]
Generic vdm_JNIEnv::GetStringChars(const Sequence & p)
{
  jstring js = static_cast<jstring>(vdm_JObject::GetPtr(static_cast<DLObject>(p[1])));

  this->env->ExceptionClear();
  jsize len = this->env->GetStringLength(js);
  jthrowable ex = this->env->ExceptionOccurred();
  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  const jchar * outbuf = this->env->GetStringChars(js, NULL);
  ex = this->env->ExceptionOccurred();
  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  Sequence res;
  for (size_t i = 0; i < len; i++) {
    res.ImpAppend(Char(outbuf[i]));
  }
  this->env->ReleaseStringChars(js, outbuf);
  ex = this->env->ExceptionOccurred();
  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    return mk_(res, Nil());
  }
}

// SetJValue: seq of jvalue * seq of jvalue * seq of char ==> seq of jvalue
void vdm_JNIEnv::SetJValue(jvalue * jv, const Sequence & args, const SEQ< SEQ<Char> > & sign)
{
  size_t len_args = args.Length();
  for (size_t i = 1; i <= len_args; i++) {
    wstring s (sign[i].GetString());

    if (s == L"Z") {
      jv[i-1].z = Bool(args[i]).GetValue();
    }
    else if (s == L"B") {
      jv[i-1].b = Int(args[i]).GetValue();
    }
    else if (s == L"C") {
      jv[i-1].c = Char(args[i]).GetValue();
    }
    else if (s == L"S") {
      jv[i-1].s = Int(args[i]).GetValue();
    }
    else if (s == L"I") {
      jv[i-1].i = Int(args[i]).GetValue();
    }
    else if (s == L"J") {
      jv[i-1].j = Int(args[i]).GetValue();
    }
    else if (s == L"F") {
      jv[i-1].f = Real(args[i]).GetValue();
    }
    else if (s == L"D") {
      jv[i-1].d = Real(args[i]).GetValue();
    }
    else if (args[i].IsNil()) {
      jv[i-1].l = NULL;
    }
    else if (args[i].IsDLObject()) {
      jv[i-1].l = vdm_JObject::GetPtr(static_cast<DLObject>(args[i]));
    }
    else {
      jv[i-1].l = NULL; // dummy 
    }
  }
}

// MakeDLObject: JNIEnv * jobject * seq of char -> [objectref]
Generic vdm_JNIEnv::MakeDLObject(JNIEnv *e, jobject obj, const SEQ<Char> & restp)
{
  if (obj == NULL) {
    return Nil();
  }
  if (restp == SEQ<Char>(L"Ljava/lang/String;")) {
    return DLObject(L"jstring", new vdm_JString(e, obj, restp));
  }
  else if (restp == SEQ<Char>(L"Ljava/lang/Class;")) {
    return DLObject(L"jclass", new vdm_JClass(e, obj, restp));
  }
  else if (restp == SEQ<Char>(L"Ljava/lang/Throwable;")) {
    return DLObject(L"jthrowable", new vdm_JClass(e, obj, restp));
  }
  if (restp == SEQ<Char>(L"[Z")) {
    return DLObject(L"jbooleanArray", new vdm_JBooleanArray(e, obj, restp));
  }
  else if (restp == SEQ<Char>(L"[B")) {
    return DLObject(L"jbyteArray", new vdm_JByteArray(e, obj, restp));
  }
  else if (restp == SEQ<Char>(L"[C")) {
    return DLObject(L"jcharArray", new vdm_JCharArray(e, obj, restp));
  }
  else if (restp == SEQ<Char>(L"[S")) {
    return DLObject(L"jshortArray", new vdm_JShortArray(e, obj, restp));
  }
  else if (restp == SEQ<Char>(L"[I")) {
    return DLObject(L"jintArray", new vdm_JIntArray(e, obj, restp));
  }
  else if (restp == SEQ<Char>(L"[J")) {
    return DLObject(L"jlongArray", new vdm_JLongArray(e, obj, restp));
  }
  else if (restp == SEQ<Char>(L"[F")) {
    return DLObject(L"jfloatArray", new vdm_JFloatArray(e, obj, restp));
  }
  else if (restp == SEQ<Char>(L"[D")) {
    return DLObject(L"jdoubleArray", new vdm_JDoubleArray(e, obj, restp));
  }
  else if (restp[1] == Char(L'[')) {
    return DLObject(L"jobjectArray", new vdm_JObjectArray(e, obj, restp));
  }
  else {
    return DLObject(L"jobject", new vdm_JObject(e, obj, restp));
  }
}

Generic vdm_JNIEnv::CallNoArgObjectMethod(JNIEnv *e, jobject obj, const SEQ<Char> & mtnm, const SEQ<Char> & restp)
{
  string mnm (wstring2utf8str(mtnm.GetString()));
  string sign ("()" + wstring2utf8str(restp.GetString()));

  e->ExceptionClear();
  jclass cls = e->GetObjectClass(obj);
  jthrowable ex = e->ExceptionOccurred();
  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(e, ex));
  }
  jmethodID mid = e->GetMethodID(cls, mnm.c_str(), sign.c_str());
  ex = e->ExceptionOccurred();
  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(e, ex));
  }
  jobject resobj = e->CallObjectMethod(obj, mid, NULL);
  ex = e->ExceptionOccurred();
  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(e, ex));
  }
  return mk_(vdm_JNIEnv::MakeDLObject(e, resobj, restp), Nil());
}

// ConvSign: seq of char -> bool * seq of seq of char * seq of char
Tuple vdm_JNIEnv::ConvSign(const Sequence & sign)
{
  wstring s (sign.GetString());
  std::string::size_type lp1 = s.find(L"(");
  std::string::size_type lp2 = s.rfind(L"(");
  std::string::size_type rp1 = s.find(L")");
  std::string::size_type rp2 = s.rfind(L")");
  if ((lp1 == wstring::npos) || (lp1 != lp2) ||
      (rp1 == wstring::npos) || (lp1 != lp2) ||
      (lp1 > rp1)) {
    return mk_(Bool(false), Nil(), Nil());
  }
  wstring domstr (s.substr(lp1 + 1,rp1 - lp1 - 1));

  Sequence domtp;
  if (!domstr.empty()) {
    Sequence domseq (SignStringToSeq(domstr));
    if (domseq.IsEmpty()) {
      return mk_(Bool(false), Nil(), Nil());
    }
    domtp.ImpConc(SignStringToSeq(domstr));
  }
  
  wstring resstr (s.substr(rp1 + 1));
  if (resstr.empty()) {
    return mk_(Bool(false), Nil(), Nil());
  }
  Sequence resseq (SignStringToSeq(resstr));
  if (resseq.Length() != 1) {
    return mk_(Bool(false), Nil(), Nil());
  }
  return mk_(Bool(true), domtp, resseq[1]);
}

// SignStringToSeq: seq of char -> seq of seq of char
Sequence vdm_JNIEnv::SignStringToSeq(const wstring & str)
{
  int len_sign = str.length();
   
  Sequence res;
  int index = 0;
  bool objecttype = false;
  wstring tmp (L"");
  for (int index = 0; index < len_sign; index++) {
    wchar_t c (str[index]);
    if (objecttype) {
      tmp += c;
      if (c == L';') {
        objecttype = false;
        res.ImpAppend(Sequence(tmp));
        tmp = L"";
      }
    }
    else {
      switch (c) {
        case L'[': {
          tmp += c;
          break;
        }
        case L'L': {
          tmp += c;
          objecttype = true;
          break;
        }
        case L'V':
        case L'Z':
        case L'B':
        case L'C':
        case L'S':
        case L'I':
        case L'J':
        case L'F':
        case L'D': {
          tmp += c;
          res.ImpAppend(Sequence(tmp));
          tmp = L"";
          break;
        }
      }
    }
  }
  if (!tmp.empty() || objecttype) {
    return Sequence();
  }
  return res;
}

//-----------

vdm_JObject::vdm_JObject(JNIEnv *e, jobject obj, const SEQ<Char> & sign) : env(e)
{
  this->_obj = e->NewGlobalRef(obj);
  e->DeleteLocalRef(obj);
  this->classTp = sign;
}

vdm_JObject::~vdm_JObject()
{
  this->env->DeleteGlobalRef(_obj);
}

Generic vdm_JObject::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  if (wcscmp(name, L"toString_i")==0) {
    return toString();
  }
  else if (wcscmp(name, L"equals_i")==0) {
    return equals(p);
  }
  else if (wcscmp(name, L"getClass_i")==0) {
    return getClass();
  }
  return Generic();
}

Generic vdm_JObject::toString()
{
  return vdm_JNIEnv::CallNoArgObjectMethod(this->env, this->_obj, SEQ<Char>(L"toString"),
                                           SEQ<Char>(L"Ljava/lang/String;"));
}

Generic vdm_JObject::equals(const Sequence & p)
{
  this->env->ExceptionClear();
  jclass cls = this->env->GetObjectClass(this->_obj);
  jthrowable ex = this->env->ExceptionOccurred();
  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  jmethodID mid = this->env->GetMethodID(cls, "equals", "(Ljava/lang/Object;)Z");
  ex = this->env->ExceptionOccurred();
  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  jobject obj = vdm_JObject::GetPtr(static_cast<DLObject>(p[1]));
  jboolean res = this->env->CallBooleanMethod(this->_obj, mid, obj);
  ex = this->env->ExceptionOccurred();
  if (ex != NULL) {
    return mk_(Nil(), THROWABLE(this->env, ex));
  }
  else {
    return mk_(Bool(res), Nil());
  }
}

Generic vdm_JObject::getClass()
{
  return vdm_JNIEnv::CallNoArgObjectMethod(this->env, this->_obj, SEQ<Char>(L"getClass"),
                                           SEQ<Char>(L"Ljava/lang/Class;"));
}

Generic vdm_JClass::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  if (wcscmp(name, L"getName_i")==0) {
    return getName();
  }
  return vdm_JObject::DlMethodCall(name, p);
}

Generic vdm_JClass::getName()
{
  return vdm_JNIEnv::CallNoArgObjectMethod(this->env, this->_obj, SEQ<Char>(L"getName"),
                                           SEQ<Char>(L"Ljava/lang/String;"));
}

Generic vdm_JFieldID::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  return Generic();
}

Generic vdm_JMethodID::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  return Generic();
}

Generic vdm_JThrowable::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  if (wcscmp(name, L"getMessage_i")==0) {
    return getMessage();
  }
  else if (wcscmp(name, L"getStackTrace_i")==0) {
    return getStackTrace();
  }
  return vdm_JObject::DlMethodCall(name, p);
}

Generic vdm_JThrowable::getMessage()
{
  return vdm_JNIEnv::CallNoArgObjectMethod(this->env, this->_obj, SEQ<Char>(L"getMessage"),
                                           SEQ<Char>(L"Ljava/lang/String;"));
}

Generic vdm_JThrowable::getStackTrace()
{
  return vdm_JNIEnv::CallNoArgObjectMethod(this->env, this->_obj, SEQ<Char>(L"getStackTrace"),
                                           SEQ<Char>(L"[Ljava/lang/StackTraceElement;"));
}

Generic vdm_JString::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  return vdm_JObject::DlMethodCall(name, p);
}

Generic vdm_JArray::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  return vdm_JObject::DlMethodCall(name, p);
}

Generic vdm_JBooleanArray::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  return vdm_JArray::DlMethodCall(name, p);
}

Generic vdm_JByteArray::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  return vdm_JArray::DlMethodCall(name, p);
}

Generic vdm_JCharArray::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  return vdm_JArray::DlMethodCall(name, p);
}

Generic vdm_JShortArray::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  return vdm_JArray::DlMethodCall(name, p);
}

Generic vdm_JIntArray::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  return vdm_JArray::DlMethodCall(name, p);
}

Generic vdm_JLongArray::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  return vdm_JArray::DlMethodCall(name, p);
}

Generic vdm_JFloatArray::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  return vdm_JArray::DlMethodCall(name, p);
}

Generic vdm_JDoubleArray::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  return vdm_JArray::DlMethodCall(name, p);
}

Generic vdm_JObjectArray::DlMethodCall (const wchar_t* name, const Sequence &p)
{
  return vdm_JArray::DlMethodCall(name, p);
}

