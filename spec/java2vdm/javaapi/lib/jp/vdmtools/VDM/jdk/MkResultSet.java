/*
 * Created by IntelliJ IDEA.
 * User: paulm
 * Date: Nov 22, 2001
 * Time: 10:17:44 AM
 * To change template for new class use
 * Code Style | Class Templates options (Tools | IDE Options).
 */
package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.VDMSpecException;

import java.sql.*;
import java.util.HashMap;

public class MkResultSet extends JavaLangObject implements JavaSqlResultSet {

    private ResultSet wrappedObject;

    public MkResultSet(ResultSet rs){
        wrappedObject = rs;
    }

    public Boolean IsNil() {
        return isNil;
    }

    public Boolean last() throws CGException {
        try {
            return new Boolean(wrappedObject.last());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean next() throws CGException {
        try {
            return new Boolean(wrappedObject.next());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void close()throws CGException {
        try {
            wrappedObject.close();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean first()throws CGException {
        try {
            return new Boolean(wrappedObject.first());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getRow()throws CGException {
        try {
            return new Integer(wrappedObject.getRow());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean isLast()throws CGException {
        try {
            return new Boolean(wrappedObject.isLast());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getInt(final Integer columnIndex)throws CGException {
        try {
            return new Integer(wrappedObject.getInt(columnIndex.intValue()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getType()throws CGException {
        try {
            return new Integer(wrappedObject.getType());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean isFirst()throws CGException {
        try {
            return new Boolean(wrappedObject.isFirst());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean wasNull()throws CGException {
        try {
            return new Boolean(wrappedObject.wasNull());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getByte(final Integer columnIndex)throws CGException {
        try {
            return new Integer(wrappedObject.getByte(columnIndex.intValue()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getLong(final Integer columnIndex)throws CGException {
        try {
            return new Integer((int) wrappedObject.getLong(columnIndex.intValue()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean previous()throws CGException {
        try {
            return new Boolean(wrappedObject.previous());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean absolute(final Integer row)throws CGException {
        try {
            return new Boolean(wrappedObject.absolute(row.intValue()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void afterLast()throws CGException {
        try {
            wrappedObject.afterLast();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void deleteRow()throws CGException {
        try {
            wrappedObject.deleteRow();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public HashMap getBytes(final Integer columnIndex)throws CGException {
        try {
            byte[] bytes = wrappedObject.getBytes(columnIndex.intValue());
            return bytesToHashMap(bytes);
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    private HashMap bytesToHashMap(byte[] bytes)
    throws CGException {
        HashMap result = new HashMap();
        for(int i = 0; i < bytes.length; i++){
            result.put(new Integer(i), new Integer((int) bytes[i]));
        }
        return result;
    }

    public Double getFloat(final Integer columnIndex)throws CGException {
        try {
            return new Double(wrappedObject.getFloat(columnIndex.intValue()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getShort(final Integer columnIndex)throws CGException {
        try {
            return new Integer((int) wrappedObject.getShort(columnIndex.intValue()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void insertRow()throws CGException {
        try {
            wrappedObject.insertRow();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean relative(final Integer rows)throws CGException {
        try {
            return new Boolean(wrappedObject.relative(rows.intValue()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateRow()throws CGException {
        try {
            wrappedObject.updateRow();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Double getDouble(final Integer columnIndex)throws CGException {
        try {
            return new Double(wrappedObject.getDouble(columnIndex.intValue()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public JavaLangString getString(final Integer columnIndex)throws CGException {
        try {
            return new JavaLangString(wrappedObject.getString(columnIndex.intValue()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void refreshRow()throws CGException {
        try {
            wrappedObject.refreshRow();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean rowDeleted()throws CGException {
        try {
            return new Boolean (wrappedObject.rowDeleted());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean rowUpdated()throws CGException {
        try {
            return new Boolean(wrappedObject.rowUpdated());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void beforeFirst()throws CGException {
        try {
            wrappedObject.beforeFirst();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean getBoolean(final Integer columnIndex)throws CGException {
        try {
            return new Boolean(wrappedObject.getBoolean(columnIndex.intValue()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean isAfterLast()throws CGException {
        try {
            return new Boolean(wrappedObject.isAfterLast());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean rowInserted()throws CGException {
        try {
            return new Boolean(wrappedObject.rowInserted());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateNull(final Integer columnIndex)throws CGException {
        try {
            wrappedObject.updateNull(columnIndex.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getFetchSize()throws CGException {
        try {
            return new Integer(wrappedObject.getFetchSize());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public JavaSqlStatement getStatement()throws CGException {
        try {
            return new MkStatement(wrappedObject.getStatement());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateInt(final Integer columnIndex, final Integer x)throws CGException {
        try {
            wrappedObject.updateInt(columnIndex.intValue(), x.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void clearWarnings()throws CGException {
        try {
            wrappedObject.clearWarnings();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public JavaLangString getCursorName()throws CGException {
        try {
            return new JavaLangString(wrappedObject.getCursorName());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean isBeforeFirst()throws CGException {
        try {
            return new Boolean(wrappedObject.isBeforeFirst());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void setFetchSize(final Integer rows)throws CGException {
        try {
            wrappedObject.setFetchSize(rows.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateByte(final Integer columnIndex, final Integer x)throws CGException {
        try {
            wrappedObject.updateByte(columnIndex.intValue(), (byte) x.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateLong(final Integer columnIndex, final Integer x)throws CGException {
        try {
            wrappedObject.updateLong(columnIndex.intValue(), x.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getConcurrency()throws CGException {
        try {
            return new Integer(wrappedObject.getConcurrency());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getInt(final JavaLangString columnName)throws CGException {
        try {
            return new Integer(wrappedObject.getInt(columnName.toSeqOfChar()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateFloat(final Integer columnIndex, final Double x)throws CGException {
        try {
            wrappedObject.updateFloat(columnIndex.intValue(), x.floatValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateShort(final Integer columnIndex, final Integer x)throws CGException {
        try {
            wrappedObject.updateShort(columnIndex.intValue(), (short) x.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getByte(final JavaLangString columnName)throws CGException {
        try {
            return new Integer(wrappedObject.getByte(columnName.toSeqOfChar()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getLong(final JavaLangString columnName)throws CGException {
        try {
            return new Integer((int) wrappedObject.getLong(columnName.toSeqOfChar()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void moveToInsertRow()throws CGException {
        try {
            wrappedObject.moveToInsertRow();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateDouble(final Integer columnIndex, final Double x)throws CGException {
        try {
            wrappedObject.updateDouble(columnIndex.intValue(),x.doubleValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void cancelRowUpdates()throws CGException {
        try {
            wrappedObject.cancelRowUpdates();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public HashMap getBytes(final JavaLangString columnName)throws CGException {
        try {
            byte[] bytes = wrappedObject.getBytes(columnName.toSeqOfChar());
            return bytesToHashMap(bytes);
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Double getFloat(final JavaLangString columnName)throws CGException {
        try {
            return new Double(wrappedObject.getFloat(columnName.toSeqOfChar()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getShort(final JavaLangString columnName)throws CGException {
        try {
            return new Integer((int) wrappedObject.getShort(columnName.toSeqOfChar()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void moveToCurrentRow()throws CGException {
        try {
            wrappedObject.moveToCurrentRow();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateBoolean(final Integer columnIndex, final Boolean x)throws CGException {
        try {
            wrappedObject.updateBoolean(columnIndex.intValue(), x.booleanValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Double getDouble(final JavaLangString columnName)throws CGException {
        try {
            return new Double(wrappedObject.getDouble(columnName.toSeqOfChar()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getFetchDirection()throws CGException {
        try {
            return new Integer(wrappedObject.getFetchDirection());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public JavaLangObject getObject(final JavaLangString columnName)throws CGException {
        try {
            return (JavaLangObject) wrappedObject.getObject(columnName.toSeqOfChar());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public JavaLangString getString(final JavaLangString columnName)throws CGException {
        try {
            return new JavaLangString(wrappedObject.getString(columnName.toSeqOfChar()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer findColumn(final JavaLangString columnName)throws CGException {
        try {
            return new Integer(wrappedObject.findColumn(columnName.toSeqOfChar()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean getBoolean(final JavaLangString columnName)throws CGException {
        try {
            return new Boolean(wrappedObject.getBoolean(columnName.toSeqOfChar()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void setFetchDirection(final Integer direction)throws CGException {
        try {
            wrappedObject.setFetchDirection(direction.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateBytes(final Integer columnIndex, final HashMap x)throws CGException {
        try {
            wrappedObject.updateBytes(columnIndex.intValue(), hashMapToBytes(x));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    private byte[] hashMapToBytes(HashMap hm)throws CGException {
        byte[] bytes = new byte[hm.size()];
        for (int i = 0; i < bytes.length; i++)
        {
            bytes[i] = (byte) ((Integer) hm.get(new Integer(i))).intValue();
        }
        return bytes;
    }

    public void updateNull(final JavaLangString columnName)throws CGException {
        try {
            wrappedObject.updateNull(columnName.toSeqOfChar());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateInt(final JavaLangString columnName,
                          final Integer x)
    throws CGException {
        try {
            wrappedObject.updateInt(columnName.toSeqOfChar(), x.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateByte(final JavaLangString columnName,
                           final Integer x)
    throws CGException {
        try {
            wrappedObject.updateByte(columnName.toSeqOfChar(), (byte) x.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateLong(final JavaLangString columnName,
                           final Integer x)
    throws CGException {
        try {
            wrappedObject.updateLong(columnName.toSeqOfChar(), x.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }


    public void updateFloat(final JavaLangString columnName,
                            final Double x)
    throws CGException {
        try {
            wrappedObject.updateFloat(columnName.toSeqOfChar(), (float) x.doubleValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }


    public void updateShort(final JavaLangString columnName,
                            final Integer x)
    throws CGException {
        try {
            wrappedObject.updateShort(columnName.toSeqOfChar(), (short) x.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }


    public void updateDouble(final JavaLangString columnName,
                             final Double x)
    throws CGException {
        try {
            wrappedObject.updateDouble(columnName.toSeqOfChar(), x.doubleValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }


    public void updateObject(final Integer columnIndex,
                             final JavaLangObject x)
    throws CGException {
        try {
            wrappedObject.updateObject(columnIndex.intValue(), x);
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateString(final Integer columnIndex,
                             final JavaLangString x)
    throws CGException {
        try {
            wrappedObject.updateString(columnIndex.intValue(), x.toSeqOfChar());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateBoolean(final JavaLangString columnName,
                              final Boolean x)
    throws CGException {
        try {
            wrappedObject.updateBoolean(columnName.toSeqOfChar(), x.booleanValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateBytes(final JavaLangString columnName,
                            final HashMap x)
    throws CGException {
        try {
            wrappedObject.updateBytes(columnName.toSeqOfChar(), hashMapToBytes(x));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void updateString(final JavaLangString columnName,
                             final JavaLangString x)
    throws CGException {
        try {
            wrappedObject.updateString(columnName.toSeqOfChar(), x.toSeqOfChar());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }
}
