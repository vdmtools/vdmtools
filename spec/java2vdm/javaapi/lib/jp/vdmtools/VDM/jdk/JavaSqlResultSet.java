package jp.vdmtools.VDM.jdk;

import java.util.HashMap;
import jp.vdmtools.VDM.CGException;

public interface JavaSqlResultSet extends Nullable {

    public static final Integer FETCH_uFORWARD = Integer.valueOf(1000);
    public static final Integer FETCH_uREVERSE = Integer.valueOf(1001);
    public static final Integer FETCH_uUNKNOWN = Integer.valueOf(1002);
    public static final Integer TYPE_uFORWARD_uONLY = Integer.valueOf(1003);
    public static final Integer TYPE_uSCROLL_uINSENSITIVE = Integer.valueOf(1004);
    public static final Integer TYPE_uSCROLL_uSENSITIVE = Integer.valueOf(1005);
    public static final Integer CONCUR_uREAD_uONLY = Integer.valueOf(1007);
    public static final Integer CONCUR_uUPDATABLE = Integer.valueOf(1008);

    abstract public Boolean last() throws CGException;

    abstract public Boolean next()throws CGException ;

    abstract public void close()throws CGException ;

    abstract public Boolean first()throws CGException ;

    abstract public Integer getRow()throws CGException ;

    abstract public Boolean isLast()throws CGException ;

    abstract public Integer getInt(final Integer columnIndex)throws CGException ;

    abstract public Integer getType()throws CGException ;

    abstract public Boolean isFirst()throws CGException ;

    abstract public Boolean wasNull()throws CGException ;

    abstract public Integer getByte(final Integer columnIndex)throws CGException ;

    abstract public Integer getLong(final Integer columnIndex)throws CGException ;

    abstract public Boolean previous()throws CGException ;

    abstract public Boolean absolute(final Integer row)throws CGException ;

    abstract public void afterLast()throws CGException ;

    abstract public void deleteRow()throws CGException ;

    abstract public HashMap getBytes(final Integer columnIndex)throws CGException ;

    abstract public Double getFloat(final Integer columnIndex)throws CGException ;

    abstract public Integer getShort(final Integer columnIndex)throws CGException ;

    abstract public void insertRow()throws CGException ;

    abstract public Boolean relative(final Integer rows)throws CGException ;

    abstract public void updateRow()throws CGException ;

    abstract public Double getDouble(final Integer columnIndex)throws CGException ;

    abstract public JavaLangString getString(final Integer columnIndex)throws CGException ;

    abstract public void refreshRow()throws CGException ;

    abstract public Boolean rowDeleted()throws CGException ;

    abstract public Boolean rowUpdated()throws CGException ;

    abstract public void beforeFirst()throws CGException ;

    abstract public Boolean getBoolean(final Integer columnIndex)throws CGException ;

    abstract public Boolean isAfterLast()throws CGException ;

    abstract public Boolean rowInserted()throws CGException ;

    abstract public void updateNull(final Integer columnIndex)throws CGException ;

    abstract public Integer getFetchSize()throws CGException ;

    abstract public JavaSqlStatement getStatement()throws CGException ;

    abstract public void updateInt(final Integer columnIndex, final Integer x)throws CGException ;

    abstract public void clearWarnings()throws CGException ;

    abstract public JavaLangString getCursorName()throws CGException ;

    abstract public Boolean isBeforeFirst()throws CGException ;

    abstract public void setFetchSize(final Integer rows)throws CGException ;

    abstract public void updateByte(final Integer columnIndex, final Integer x)throws CGException ;

    abstract public void updateLong(final Integer columnIndex, final Integer x)throws CGException ;

    abstract public Integer getConcurrency()throws CGException ;

    abstract public Integer getInt(final JavaLangString columnName)throws CGException ;

    abstract public void updateFloat(final Integer columnIndex, final Double x)throws CGException ;

    abstract public void updateShort(final Integer columnIndex, final Integer x)throws CGException ;

    abstract public Integer getByte(final JavaLangString columnName)throws CGException ;

    abstract public Integer getLong(final JavaLangString columnName)throws CGException ;

    abstract public void moveToInsertRow()throws CGException ;

    abstract public void updateDouble(final Integer columnIndex, final Double x)throws CGException ;

    abstract public void cancelRowUpdates()throws CGException ;

    abstract public HashMap getBytes(final JavaLangString columnName)throws CGException ;

    abstract public Double getFloat(final JavaLangString columnName)throws CGException ;

    abstract public Integer getShort(final JavaLangString columnName)throws CGException ;

    abstract public void moveToCurrentRow()throws CGException ;

    abstract public void updateBoolean(final Integer columnIndex, final Boolean x)throws CGException ;

    abstract public Double getDouble(final JavaLangString columnName)throws CGException ;

    abstract public Integer getFetchDirection()throws CGException ;

    abstract public JavaLangObject getObject(final JavaLangString columnName)throws CGException ;

    abstract public JavaLangString getString(final JavaLangString columnName)throws CGException ;

    abstract public Integer findColumn(final JavaLangString columnName)throws CGException ;

    abstract public Boolean getBoolean(final JavaLangString columnName)throws CGException ;

    abstract public void setFetchDirection(final Integer direction)throws CGException ;

    abstract public void updateBytes(final Integer columnIndex, final HashMap x)throws CGException ;

    abstract public void updateNull(final JavaLangString columnName)throws CGException ;

    abstract public void updateInt(final JavaLangString columnName,
                                   final Integer x)throws CGException ;

    abstract public void updateByte(final JavaLangString columnName,
                                    final Integer x)throws CGException ;

    abstract public void updateLong(final JavaLangString columnName,
                                    final Integer x)throws CGException ;

    abstract public void updateFloat(final JavaLangString columnName,
                                     final Double x)throws CGException ;

    abstract public void updateShort(final JavaLangString columnName,
                                     final Integer x)throws CGException ;

    abstract public void updateDouble(final JavaLangString columnName,
                                      final Double x)throws CGException ;

    abstract public void updateObject(final Integer columnIndex,
                                      final JavaLangObject x)throws CGException ;

    abstract public void updateString(final Integer columnIndex,
                                      final JavaLangString x)throws CGException ;

    abstract public void updateBoolean(final JavaLangString columnName,
                                       final Boolean x)throws CGException ;

    abstract public void updateBytes(final JavaLangString columnName,
                                     final HashMap x)throws CGException ;

    abstract public void updateString(final JavaLangString columnName,
                                      final JavaLangString x)throws CGException ;
}

