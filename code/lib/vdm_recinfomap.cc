/***
*  * WHAT
*  *    Implementation of RecordInfoMap
*  * FILE
*  *    $Source:$
*  * VERSION
*  *    $Revision:$
*  * DATE
*  *    $Date:$
*  * FORMAT
*  *    $State:$
*  * PROJECT
*  *    VDM-SL Toolbox
*  * STATUS
*  *    Under development.
*  * AUTHOR
*  *    $Author:$
*  * COPYRIGHT
*  *    (C) Kyushu University
***/

#include <sstream>
#include <map>
#include "metaiv.h"
#include "m4lib_errmsg.h"

/*
class VDM_RecInfo
{
//public:
//  typedef void (*vdm_pp_function_ptr)(wostream&, const Record&, VDMFormatter vf,
//                                      bool simple);
private:
#ifdef _MSC_VER
  unsigned __int64 bits;
#else
  unsigned long long bits;
#endif
  vector<bool> *bv;
  static const int BitLimit;
  int Tag;
  int Size;
  wstring symname;
  VDMRecInfoMap::vdm_pp_function_ptr print_function;

public:
  VDM_RecInfo() : bits(0), bv(NULL), Tag(0), Size(0), print_function(NULL) {};
  VDM_RecInfo(int tag, int size) : bits(0), bv(NULL), Tag(tag), Size(size), print_function(NULL) {};
  ~VDM_RecInfo() {}

  bool AskDontCare(int field) const;
  void SetDontCare(int field);
  int GetTag() const { return this->Tag; };
  bool Is(const int tag) const { return (this->Tag == tag); };
  void SetSymTag(const wstring & s) { this->symname = s; };
  const wstring & GetSymTag() const { return this->symname; };
  int GetSize() const { return this->Size; };
  bool IsEmpty() const { return (this->Size == 0); };
  bool OutOfRange(int i) const { return ((i < 1 ) || (i > this->Size)); };

  void SetPrintFunction(VDMRecInfoMap::vdm_pp_function_ptr f) { this->print_function = f; };
  bool HasPrintFunction() const { return (this->print_function != NULL); };
  void Print(wostream & os, const Record & r, VDMFormatter vf, bool simple) const
  { (*this->print_function)(os, r, vf, simple); };
  void dump(wostream & os) const
  {
    os << L"tag=" << this->Tag
       << L" size=" << this->Size
       << L" symtag=`" << this->symname << L"'";
  }
};
*/

////////////////////////////////////////////////////////////
// VDMRecInfoMap
////////////////////////////////////////////////////////////
VDM_RecInfo * VDMRecInfoMap::Register(int tag, int size)
{
  pair<VDMRecInfoMap_t::iterator, bool> p (this->mapvalue.insert(VDMRecInfoMap_t::value_type (tag, (VDM_RecInfo*)NULL)));

  if (p.second == false)
  {
    // Existed
    if ((*(p.first)).second->GetSize() != size) {
      wostringstream err;
      err << L"Inconsistent record size:"
          << L" tag=" << tag
          << L" symtag=`" << ((*p.first)).second->GetSymTag() << L"' "
          << L" known size=" << ((*p.first)).second->GetSize()
          << L" new size=" << size
          << endl;
      wstring e(err.str());
      M4LibError::ReportError(ML_WRONG_RECORD_SIZE, e.c_str());
    }
    return (*(p.first)).second;
  }
  else
  {
    // Initalize
#ifdef SHOW_MALLOC_STATS
    *liblog << L"Register: new " << Tag << "," << Size << "\n";
#endif // SHOW_MALLOC_STATS
    VDM_RecInfo * vp = new VDM_RecInfo(tag, size);
    (*(p.first)).second = vp;
    return vp;
  }
}

VDM_RecInfo * VDMRecInfoMap::Register(int tag, int size, const wstring & symtag, const vector<int> & dc)
{
  VDM_RecInfo * ri = Register(tag, size);
  ri->SetSymTag(symtag);
  this->symtagmap.insert(VDMRecInfoSymTagMap_t::value_type(symtag, ri));
  for (vector<int>::const_iterator viter = dc.begin(); viter != dc.end(); viter++)
    ri->SetDontCare(*viter - 1);
  return ri;
}

void VDMRecInfoMap::clear()
{
  this->symtagmap.clear();
  for (VDMRecInfoMap_t::iterator miter = this->mapvalue.begin(); miter != this->mapvalue.end(); ++miter)
  {
    if ((*miter).second != NULL)
    {
      delete (*miter).second;
      (*miter).second = NULL;
    }
  }
  this->mapvalue.clear();
}

bool VDMRecInfoMap::AskDontCare(int tag, int field) const
{
  VDMRecInfoMap_t::const_iterator miter = this->mapvalue.find(tag);
  if (miter != this->mapvalue.end()) {
    return (*miter).second->AskDontCare(field-1); //
  } else {
    M4LibError::ReportError(ML_TAG_NOT_FOUND,L"AskDontCare: no such tag.");
    return false;
  }
}

int VDMRecInfoMap::GetSize(int tag) const
{
  VDMRecInfoMap_t::const_iterator miter = this->mapvalue.find(tag);
  if (miter != this->mapvalue.end()) {
    return (*miter).second->GetSize();
  } else {
    M4LibError::ReportError(ML_TAG_NOT_FOUND, L"GetSize: no such tag.");
    return -1;
  }
}

bool VDMRecInfoMap::GetSymTag(int tag, wstring &s) const
{
  VDMRecInfoMap_t::const_iterator miter = this->mapvalue.find(tag);
  if (miter != this->mapvalue.end()) {
    s = (*miter).second->GetSymTag();
    return true;
  } else {
    return false;
  }
}

bool VDMRecInfoMap::GetTag(const wstring & symtag, int & tag, int & size) const
{
  VDMRecInfoSymTagMap_t::const_iterator miter (this->symtagmap.find(symtag));
  if (miter != this->symtagmap.end())
  {
    tag = (*miter).second->GetTag();
    size = (*miter).second->GetSize();
    return true;
  }
  return false;
}

bool VDMRecInfoMap::IsDecl(const wstring & symtag) const
{
  VDMRecInfoSymTagMap_t::const_iterator miter (this->symtagmap.find(symtag));
  return (miter != this->symtagmap.end());
}

const VDM_RecInfo * VDMRecInfoMap::GetInfo(const wstring & symtag) const
{
  VDMRecInfoSymTagMap_t::const_iterator miter (this->symtagmap.find(symtag));
  if (miter == this->symtagmap.end())
    M4LibError::ReportError(ML_TAG_NOT_FOUND, L"GetInfo: no such tag.");
  return (*miter).second;
}

void VDMRecInfoMap::SetDontCare(int tag, int field)
{
  VDMRecInfoMap_t::iterator miter = this->mapvalue.find(tag);
  if (miter != this->mapvalue.end()) {
    // Found.
    //(*miter).second->SetDontCare(field); //
    (*miter).second->SetDontCare(field - 1); //
  } else {
    M4LibError::ReportError(ML_TAG_NOT_FOUND, L"SetDontCare: no such tag.");
  }
}

void VDMRecInfoMap::SetSymTag(int tag, const wstring & s)
{
  VDMRecInfoMap_t::iterator miter = this->mapvalue.find(tag);
  if (miter != this->mapvalue.end()) {
    (*miter).second->SetSymTag(s); //
//
    this->symtagmap.insert(VDMRecInfoSymTagMap_t::value_type (s, (*miter).second));
//
  } else {
    M4LibError::ReportError(ML_TAG_NOT_FOUND, L"SetSymTag: no such tag.");
  }
}

void VDMRecInfoMap::SetPrintFunction(int tag, VDM_RecInfo::vdm_pp_function_ptr f)
{
  VDMRecInfoMap_t::iterator miter = this->mapvalue.find(tag);
  if (miter != this->mapvalue.end())
  { // Found.
    (*miter).second->SetPrintFunction(f); //
  } else {
    M4LibError::ReportError(ML_TAG_NOT_FOUND, L"SetSymTag: no such tag.");
  }
}

void VDMRecInfoMap::dump(wostream & os) const
{
  if (!this->mapvalue.empty())
  {
    VDMRecInfoMap_t::const_iterator miter;
    for (miter = this->mapvalue.begin(); miter != this->mapvalue.end(); ++miter) {
      (*miter).second->dump(os);
      os << endl;
    }
  } else {
    os << L"{ |-> }" << endl;
  }
}

////////////////////////////////////////////////////////////
// VDM_RecInfo
////////////////////////////////////////////////////////////
const int VDM_RecInfo::BitLimit = 64;

VDM_RecInfo::VDM_RecInfo(int tag, int size) : bits(0), Tag(tag), Size(size), print_function(NULL), bv(NULL)
{
  if (this->Size >= BitLimit)
  {
    this->bv = new vector<bool>();
    this->bv->reserve(this->Size);
  }
};

VDM_RecInfo::~VDM_RecInfo()
{
  if (this->bv != NULL)
    delete this->bv;
}

bool VDM_RecInfo::AskDontCare(int field) const 
{
   // 0 base
  if (field < 0)
    return false;

//  if (this->bv != NULL)
  if (this->Size >= BitLimit)
    return (((unsigned)field < this->bv->size()) && (*this->bv)[field]);
  else
    return ((this->bits & (1UL << field)) != 0);
}

void VDM_RecInfo::SetDontCare(int field)
{
   // 0 base
  if (field < 0)
    return;

//  if ((field >= BitLimit) || (this->bv != NULL))
  if (this->Size >= BitLimit)
    (*this->bv)[field] = true;   
  else
    this->bits |= (1UL << field);
}
