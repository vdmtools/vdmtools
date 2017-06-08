
#ifndef __ResultSet_h__
#define __ResultSet_h__

#include "Object.h"
#include "SqlStatement.h"

#ifdef _MSC_VER
#include <windows.h>
#endif // _MSC_VER

#include "sql.h"

class vdm_SqlResultSet : public vdm_Object
{
  vdm_SqlStatement* pStmt;
  Map columnsNames; // map column names to column indeces
  Map columns;      // map column names to column info 
                    //  Column Info is a tuple of
                    //    DataType, ColumnSize, DecimalDigits, Nullable (all of Int type)
  SQLSMALLINT columnCount;  // The number of columns in the result set

  int lastWasNull;      // indicator if the last readed column had NULL value

public:
  static const wchar_t * name;

public:

  vdm_SqlResultSet (vdm_SqlStatement* pstmt = NULL);

  virtual ~vdm_SqlResultSet ();

  Generic DlMethodCall (const wchar_t* name, const Sequence &p);

  virtual const wchar_t * GetTypeName () const { return vdm_SqlResultSet::name; }

  Tuple CheckError(SQLSMALLINT handleType, SQLHANDLE handle);

  virtual Bool vdm_last () ;
  virtual Bool vdm_next () ;
  virtual void vdm_close () ;
  virtual Bool vdm_first () ;
  virtual Int vdm_getRow () ;
  virtual Bool vdm_isLast () ;
  virtual Int vdm_getType () ;
  virtual Bool vdm_isFirst () ;
  virtual Tuple vdm_wasNull () ;
  virtual Bool vdm_absolute (const Int &) ;
  virtual Bool vdm_previous () ;
  virtual Bool vdm_relative (const Int &) ;
  virtual void vdm_afterLast () ;
  virtual void vdm_deleteRow () ;
//virtual DLObject vdm_getObject (const vdm_String &) ;
  virtual void vdm_insertRow () ;
  virtual void vdm_updateRow () ;
  virtual Int vdm_findColumn (const vdm_String &) ;
  virtual Tuple vdm_getInt (const Int &) ;
  virtual void vdm_refreshRow () ;
  virtual Bool vdm_rowDeleted () ;
  virtual Bool vdm_rowUpdated () ;
  virtual void vdm_beforeFirst () ;
  virtual Int vdm_getByte (const Int &) ;
  virtual Int vdm_getLong (const Int &) ;
  virtual Bool vdm_isAfterLast () ;
  virtual Bool vdm_rowInserted () ;
  virtual type_iiM vdm_getBytes (const Int &) ;
  virtual Int vdm_getFetchSize () ;
  virtual Real vdm_getFloat (const Int &) ;
  virtual Int vdm_getShort (const Int &) ;
  virtual DLObject vdm_getStatement () ;
  virtual void vdm_setFetchSize (const Int &) ;
  virtual void vdm_updateObject (const Int &, const vdm_Object &) ;
  virtual void vdm_clearWarnings () ;
//  virtual DLObject vdm_getCursorName () ;
  virtual Real vdm_getDouble (const Int &) ;
  virtual Tuple vdm_getString (const Int &) ;
  virtual Bool vdm_isBeforeFirst () ;
  virtual Bool vdm_getBoolean (const Int &) ;
  virtual Int vdm_getConcurrency () ;
  virtual void vdm_updateNull (const Int &) ;
  virtual void vdm_moveToInsertRow () ;
  virtual void vdm_updateInt (const Int &, const Int &) ;
  virtual void vdm_cancelRowUpdates () ;
  virtual void vdm_moveToCurrentRow () ;
  virtual void vdm_updateByte (const Int &, const Int &) ;
  virtual void vdm_updateLong (const Int &, const Int &) ;
  virtual Int vdm_getFetchDirection () ;
  virtual Tuple vdm_getInt (const vdm_String &) ;
  virtual void vdm_setFetchDirection (const Int &) ;
  virtual void vdm_updateFloat (const Int &, const Real &) ;
  virtual void vdm_updateShort (const Int &, const Int &) ;
  virtual Int vdm_getByte (const vdm_String &) ;
  virtual Int vdm_getLong (const vdm_String &) ;
  virtual void vdm_updateDouble (const Int &, const Real &) ;
  virtual type_iiM vdm_getBytes (const vdm_String &) ;
  virtual Real vdm_getFloat (const vdm_String &) ;
  virtual Int vdm_getShort (const vdm_String &) ;
  virtual void vdm_updateBoolean (const Int &, const Bool &) ;
  virtual Real vdm_getDouble (const vdm_String &) ;
  virtual Tuple vdm_getString (const vdm_String &) ;
  virtual Bool vdm_getBoolean (const vdm_String &) ;
  virtual void vdm_updateBytes (const Int &, const type_iiM &) ;
  virtual void vdm_updateNull (const vdm_String &) ;
  virtual void vdm_updateInt (const vdm_String &, const Int &) ;
  virtual void vdm_updateByte (const vdm_String &, const Int &) ;
  virtual void vdm_updateLong (const vdm_String &, const Int &) ;
  virtual void vdm_updateFloat (const vdm_String &, const Real &) ;
  virtual void vdm_updateShort (const vdm_String &, const Int &) ;
  virtual void vdm_updateDouble (const vdm_String &, const Real &) ;
  virtual void vdm_updateString (const Int &, const vdm_String &) ;
  virtual void vdm_updateBoolean (const vdm_String &, const Bool &) ;
  virtual void vdm_updateBytes (const vdm_String &, const type_iiM &) ;
  virtual void vdm_updateString (const vdm_String &, const vdm_String &) ;

  void create ();

private:
  
  static vdm_SqlResultSet *GetDLPtr(const DLObject& obj);

}
;

#endif // __ResultSet_h__

