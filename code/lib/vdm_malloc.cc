/***
*  * WHAT
*  *    Implementation of malloc function
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

#include "metaiv.h"
#include <map>

/**************************************************************************
*  If ML_TRACE is on, it will be recorded in the file
*  m4trace, each time a constructor or destructor is called.
**************************************************************************/

#ifdef ML_TRACE
long MetaivVal::metaivval_object_create_count;
long Common::metaiv_object_create_count;
wstring traceHeader( MetaivVal *obj );
wstring traceHeader( Common *obj );
#endif // ML_TRACE

#ifdef ML_TRACE
wofstream* m4trace;
//int m4DebugLevel = 0;
int m4DebugLevel = 3;

int getM4DebugLevel()
{
  return m4DebugLevel;
}

// setM4DebugLevel dictates the level of debugging:
//  0: Construction and destruction of MetaivVal
//  1: Construction and destruction of Common
//  2: Construction and destruction of internal containers
//  3: Construction and destruction of metaiv values
// Of course if x > y, the x includes debug messages for level y
void setM4DebugLevel(int i)
{
  if (i >= 0 && i < 4)
    m4DebugLevel = i;
  else
    cerr << "setM4DebugLevel: argument must be between 0 and 3" << endl;
}

wofstream* getM4trace()
{
  if (!m4trace)
    m4trace = new wofstream("m4trace");

  return m4trace;
}

void Common::traceObj( const wchar_t *method )
{
  if ( m4DebugLevel > 2 )
  {
    (*getM4trace()) << ""
                    << traceHeader( this->p ) << " "
                    << method
                    << L" MetaivId=" << this->metaiv_object_id
                    << L" MetaivValId=" << this->p->metaivval_object_id
                    << endl;
    (*getM4trace()) << L""
                    << traceHeader( this ) << " "
                    << method
                    << L" MetaivId=" << this->metaiv_object_id
                    << L" MetaivValId=" << this->p->metaivval_object_id
                    << endl;
  }
}

void MetaivVal::traceObj( const wchar_t *method )
{
  if ( m4DebugLevel > 0 )
  {
    (*getM4trace()) << L""
                    << traceHeader( this ) << " "
                    << method
                    << L" Type=" << PrintType( this )
                    << L" MetaivValId=" << this->metaivval_object_id
                    << L" BuildCount=" << this->BuildCount
                    << L" RefCount=" << this->RefCount
                    << L" Refs:";
    for( unsigned int i = 0; i < this->m_list.size(); i++ )
    {
      long id = this->m_list.at(i);
      (*getM4trace()) << L" " << id;
    }
    (*getM4trace()) << endl;
  }
}

wstring traceHeader( MetaivVal *obj )
{
  wstring ret;
  wchar_t hebuf[20];
  if( obj->seq_count == 0 )
  {
    swprintf( hebuf, 20, L" %08d\n", obj->metaivval_object_id );
    ret += hebuf;
  }
  swprintf( hebuf, 20, L" %08d-%06d", obj->metaivval_object_id, obj->seq_count );
  obj->seq_count++;
  ret += hebuf;
  return ret;
}

wstring traceHeader( Common *obj )
{
  wstring ret;
  wchar_t hebuf[20];
  if( obj->seq_count == 0 )
  {
    swprintf( hebuf, 20, L"C%08d\n", obj->metaiv_object_id );
    ret += hebuf;
  }
  swprintf( hebuf, 20, L"C%08d-%06d", obj->metaiv_object_id, obj->seq_count );
  obj->seq_count++;
  ret += hebuf;
  return ret;
}
#endif //ML_TRACE

// Initialized to 0. Do not insert `= 0'.
#ifdef SHOW_MALLOC_STATS
int MetaivVal::BuildCount;
int MetaivVal::DestroyCount;
ShowMallocCount * showmalloccount; /*  */
#endif // SHOW_MALLOC_STATS

#ifdef SHOW_MALLOC_STATS

int min(int i, int j)
{
  return (i<j) ? i : j;
}
//#include <function.h>
//template <class Pair>
//struct select2nd : public unary_function<Pair, typename Pair::second_type> {
//  const typename Pair::second_type& operator()(const Pair& x) const
//  {
//    return x.second;
//  }
//};

struct vdm_addr_key {
  MetaivVal * addr; 
  vdm_addr_key(MetaivVal *p) : addr(p) { }
  vdm_addr_key() : addr(0) { }
}; 

struct vdm_addr_data {
  long buildcount; 
  vdm_addr_data(long c) : buildcount(c) { }
  vdm_addr_data() : buildcount(-1) { }
}; 

bool operator==(const vdm_addr_key & k1, const vdm_addr_key & k2)
{
  return k1.addr == k2.addr;
}

struct hash<vdm_addr_key> {
  int operator()(const vdm_addr_key&k) const { return hash<long>()((long)k.addr); }
}; 

struct get_revpair {
  pair<long, MetaivVal *>
  operator() (pair<const vdm_addr_key, vdm_addr_data>& p) 
  {
    return make_pair(p.second.buildcount, p.first.addr); 
  }
}; 

struct get_countpair {
  pair<long, MetaivVal *>
  operator() (pair<const vdm_addr_key, vdm_addr_data>& p) 
  {
    return make_pair(p.first.addr->RefCount, p.first.addr); 
  }
}; 

bool operator<(const vdm_addr_key&k1, const vdm_addr_key&k2)
{
  return k1.addr < k2.addr; 
}

typedef map<vdm_addr_key, vdm_addr_data > vdm_mem_map_t; 
typedef map<const wchar_t*, tpinf> top_map_t; 

struct tpinf 
{ 
  int size; 
  vdm_mem_map_t m; 
  unsigned int max; 
};
  
struct vdm_mem_log {
  vdm_mem_map_t m; 
  top_map_t topm; 
  wstring vdm_type_name; 
  vdm_mem_log(const wstring&s) : vdm_type_name(s) { }
  void insert(const wchar_t *s, MetaivVal *mip, int size) 
    {
      int buildcount = mip->BuildCount; 
      top_map_t::iterator i = topm.find(s); 
      if (i == topm.end()) {
        tpinf inf; 
        inf.size = size; 
        inf.max = 1; 
        inf.m.insert(vdm_mem_map_t::value_type(mip, buildcount)); 
        topm.insert(top_map_t::value_type(s, inf)); 
      } else {
        tpinf & t = (*i).second; 
        if (t.m.size()> t.max)
          t.max = t.m.size(); 
        t.m.insert(vdm_mem_map_t::value_type(mip,buildcount)); 
      }
    }
  void remove(const wchar_t *s, MetaivVal *mip) {
    top_map_t::iterator i = topm.find(s); 
    if (i == topm.end()) {
      cerr << "remove(): type not found:" << s << '\n'; 
      return;
    }
    vdm_mem_map_t & m = (*i).second.m; 
    vdm_mem_map_t::iterator  p_el = m.find(mip); 
    if (p_el == m.end()) {
      cerr << "Not in map [" << s << "] : " << mip << "\n"; 
    } else {
      m.erase(p_el); 
    }
  }  
  void report(ostream &os) 
  {
    //vdm_mem_map_t::iterator i = m.begin();     
    os.clear(); 
    os << vdm_type_name ;
    for (top_map_t::iterator i = topm.begin(); i != topm.end(); ++i) {
      vdm_mem_map_t & m = (*i).second.m; 
      os << L"Type: " << (*i).first 
         << L", size="   << (*i).second.size
         << L", max="   << (*i).second.max
         << L", lost="  << m.size()
         << L"\n"; 
      if (m.size()>0) {
        vector<long> v; 
        typedef multimap<long, MetaivVal *> m_t; 
        m_t mptrs;         
        transform(m.begin(), m.end(), inserter(mptrs, mptrs.end()), get_countpair());
        m_t::iterator i; 
        for (i=mptrs.begin(); i != mptrs.end(); ++i) {
          VDMFormatter vf = VDM_default_formatter_PR_ASCII;
          vf.SetFlag(vf.doublequoteontoken); 
          // vf.SetFlag(vf.prettyprint); 
          vf.SetIndent(10); 

          os << L"refc=" << (*i).first << " ptr=" << (void*) (*i).second << " val="; 
          (*i).second->ostream_out(os, vf); 
          os << L"\n"; 
        }
      }
      os << L"\n"; 
    }
  }
};  

class ShowMallocCount
{
public:
  static int VDM_ChangedFromString;
  static int VDM_AppendedStringToSeq;
  static int VDM_AppendedSeqToString;

#ifdef _MSC_VER
  //typedef __int64 i64_t; 
#else
  //typedef long long int i64_t;
#endif
  typedef unsigned int i64_t; 
  i64_t allocated, allocated2, freed, freed2;
  enum { mapsize=10001 };
  typedef int StatMap[mapsize+1]; 
  StatMap statistics, statistics2, 
    block_statistics, block_statistics2, 
    freed_statistics, freed_statistics2,
    roundmap;
  int ValCount[255], ValCountRemoved[255];
  wchar_t * malloc_s_start, * malloc_s_end; 
  bool safe_addtag; 
  map<int,int,less<int> > tagcounter; 
  bool reporting; 

  vdm_mem_log total_log; 

  void log_insert(const wchar_t*name, MetaivVal *p, int c) { total_log.insert(name, p, c); }

  void log_remove(const wchar_t*s, MetaivVal *p) { total_log.remove(s, p); }

  wchar_t * get_sbrk()
  {
#ifdef _MSC_VER
    return 0;
#else
    //extern "C" wchar_t * sbrk(int); 
    return sbrk(0);
#endif
  }
  
  void
  output(ostream&os, const wchar_t* type, int index, int sz, int valsz)
  {
    os.clear();
    os << setw(15) << type
       << L"size="  << setw(3)  << sz 
       << L"valsize="  << setw(3)  << valsz 
       << L"new="   << setw(7)  << ValCount[index]
       << setw(10) << ValCount[index]*valsz/1024
       << L"lost="  << setw(7)  << ValCount[index]-ValCountRemoved[index]
       << setw(10) << (ValCount[index]-ValCountRemoved[index])*valsz/1024
       << L"\n";
  }

  ShowMallocCount() : total_log(L"") 
  { 
    
    safe_addtag = true; 
    malloc_s_start = (wchar_t*)get_sbrk();
    allocated = allocated2 = 0;
    int Size = mapsize+1; 
    fill(statistics,  statistics+Size, 0); 
    fill(statistics2, statistics2+Size, 0); 
    fill(block_statistics, block_statistics+Size, 0); 
    fill(block_statistics2, block_statistics2+Size, 0); 
    fill(freed_statistics, freed_statistics+Size, 0); 
    fill(freed_statistics2, freed_statistics2+Size, 0); 
    fill(ValCount, &ValCount[255], 0);
    fill(ValCountRemoved, &ValCountRemoved[255], 0); 
    fill(&roundmap[0], &roundmap[Size], 0); 
    reporting = false; 
  }

  void print_stat_line(ostream&os, wchar_t*s, int i, int alloc1, int alloc2, int freed)
  {
    os.setf(ios::left, ios::adjustfield);
    if (alloc1==0 && alloc2==0 && freed==0)
      return; 
    os << s 
       << setw(4) << i 
       << L"->"     << setw(4)  << roundmap[i] << " "; 
    os << setw(6) << alloc1 << L" " 
       << L"alloc=" << setw(6) << alloc2 << " "
       << L"#*size=" << setw(10) << roundmap[i]*alloc2 << " "
       << L"freed=" << setw(6) << freed << " "
       << setw(6) << L"Lost=" << setw(6) << (alloc2-freed)  
       << L"\n"; 
    os.setf(ios::right, ios::adjustfield);  
  }

  void Report(ostream & os)   
  { 
    reporting = true; 
    malloc_s_end = (wchar_t*)get_sbrk();
    os << L"VDM lib finished.\n";
    os << L"Dynamic memory used = "
#ifndef _MSC_VER
       << (int)(malloc_s_end-malloc_s_start) 
#endif
       << L"\n";  
    os << L"Allocated=" 
       << allocated+allocated2 << L"=" 
       << allocated << L"+" << allocated2 << "\n"; 
    os.clear(); 
    for (int i = 0; i <=mapsize; i++) 
      {
        print_stat_line(os, L"Size  ", i, statistics[i], block_statistics[i], freed_statistics[i]); 
        print_stat_line(os, L"Size[]", i, statistics2[i], block_statistics2[i], freed_statistics2[i]); 
      }    
    output(os, L"int",       MetaivVal::mt_int,       sizeof(Int)      , sizeof(IntVal)       ); 
    output(os, L"nil",       MetaivVal::mt_nil,       sizeof(Nil)      , sizeof(NilVal)       ); 
    output(os, L"bool",      MetaivVal::mt_bool,      sizeof(Bool)     , sizeof(BoolVal)       ); 
    output(os, L"real",      MetaivVal::mt_real,      sizeof(Real)     , sizeof(RealVal)       ); 
    output(os, L"wchar_t",      MetaivVal::mt_char,      sizeof(Char)     , sizeof(CharVal)       ); 
    output(os, L"quote",     MetaivVal::mt_quote,     sizeof(Quote)    , sizeof(QuoteVal)       ); 
    output(os, L"token",     MetaivVal::mt_token,     sizeof(Token)    , sizeof(TokenVal)       ); 
    output(os, L"text",      MetaivVal::mt_text,      sizeof(Text)     , sizeof(TextVal)       ); 
    output(os, L"map",       MetaivVal::mt_map,       sizeof(Map)      , sizeof(MapVal)       ); 
    output(os, L"set",       MetaivVal::mt_set,       sizeof(Set)      , sizeof(SetVal)       ); 
    output(os, L"sequence",  MetaivVal::mt_sequence,  sizeof(Sequence) , sizeof(SequenceVal)       ); 
    output(os, L"record",    MetaivVal::mt_record,    sizeof(Record)   , sizeof(RecordVal)       ); 
    output(os, L"tuple",     MetaivVal::mt_tuple,     sizeof(Tuple)    , sizeof(TupleVal)       ); 
    output(os, L"objectref", MetaivVal::mt_objectref, sizeof(ObjectRef), sizeof(ObjectRefVal)       ); 
    output(os, L"bool",      MetaivVal::mt_bool,      sizeof(Bool)     , sizeof(BoolVal)       ); 
    os << L"Not deleted:\n"; 
    total_log.report(os); 
    os << L"Change count:\n"; 


    os << L"VDM_ChangedFromString="  << VDM_ChangedFromString << "\n"
       << L"VDM_AppendedStringToSeq" << VDM_AppendedStringToSeq << "\n"
       << L"VDM_AppendedSeqToString" << VDM_AppendedSeqToString << "\n"; 
    map<int,int,less<int> >::iterator p (tagcounter.begin()); 
    os << L"No of different tags=" << tagcounter.size() << "\n"; 
    int sum = 0; 
    for ( ; p != tagcounter.end(); ++p) {
      os << setw(7) << (*p).first << L" " << (*p).second << "\n"; 
      sum += (*p).second; 
    }
    os << L"Sum of tag counts=" << sum
      //accumulate(tagcounter.begin(), tagcounter.end(), 
      //           select2nd<map<int,int,less<int> >::value_type>())
       << L"\n"; 
  }

  void inc(int *psize, int*preal, void*p, int size, i64_t& alloc) 
  {
    if (reporting)
      return; 
    int idx = min(size, (int) mapsize); 
    if (psize) 
      psize[idx]++;
    const int sizeoffset=-1;    // works on Linux, may be -2 elsewhere
    int blocksize= (((long*)p)[sizeoffset]) & ~1; 
    roundmap[idx] = blocksize; 
    //if (blocksize==1000)
    //  { 
    //    int i=0; 
    //    i=blocksize; 
    //  }
    alloc += blocksize; 
    idx = min(blocksize, (int)mapsize); 
    preal[idx] ++; 
  }
    
    
  void add(int size, void *p) 
  { 
    inc(statistics, block_statistics, p, size, allocated); 
  }
  void addarray(int size, void *p) 
  { 
    inc(statistics, block_statistics2, p, size, allocated2); 
  }
  void delete_dec(void *p)
  {
    if (p)
      inc(0, freed_statistics, p, 0, freed); 
  }
  void delete_decarray(void *p)
  {
    if (p)
      inc(0, freed_statistics2, p, 0, freed2); 
  }

  void addval(metaivType tp) { ValCount[(int)tp] ++; }
  void remval(metaivType tp) { ValCountRemoved[(int)tp] ++; }
  void addtag(int tag) 
  {
    if (!safe_addtag)
      return; 
    safe_addtag = false; 
    pair<map<int,int,less<int> >::iterator, bool> p 
      (tagcounter.insert (pair<const int, int> (tag,0))); 
    (*(p.first)).second++; 
    //fprintf(stderr,L"mapsize=%d\n", tagcounter.size()); 
    safe_addtag = true; 
  }
  ~ShowMallocCount() 
  {
    fprintf(stderr,L"~ShowMallocCount()\n"); 
  }
};

int ShowMallocCount::VDM_ChangedFromString = 0;
int ShowMallocCount::VDM_AppendedStringToSeq = 0;
int ShowMallocCount::VDM_AppendedSeqToString = 0;
#endif //SHOW_MALLOC_STATS

#ifdef USE_OUR_OWN_OPERATOR_NEW
void * operator new(size_t sz)
{
#ifdef ML_TRACE
  //  fprintf(stderr,"new %d\n", sz); 
#endif // ML_TRACE
  void *p = malloc(sz);
#ifdef SHOW_MALLOC_STATS
  if (showmalloccount)
    showmalloccount->add(sz, p); 
#endif // SHOW_MALLOC_STATS
  return p;
}
void * operator new[](size_t sz)
{
#ifdef ML_TRACE
  //  fprintf(stderr,"new[] %d\n", sz); 
#endif // ML_TRACE
  void *p = malloc(sz);
#ifdef SHOW_MALLOC_STATS
  if (showmalloccount)
    showmalloccount->addarray(sz, p); 
#endif
  return p;
}

void operator delete  (void *p)
{
#ifdef SHOW_MALLOC_STATS
  if (showmalloccount)
    showmalloccount->delete_dec(p);
#endif
  free(p);
}

void operator delete [] (void *p)
{
#ifdef SHOW_MALLOC_STATS
  if (showmalloccount)
    showmalloccount->delete_decarray(p); 
#endif
  free(p);
}

#endif // USE_OUR_OWN_OPERATOR_NEW

