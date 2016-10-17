package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;

import java.util.HashMap;

public interface JavaSqlStatement extends Nullable {


    abstract public void close() throws CGException;

    abstract public void cancel() throws CGException;

    abstract public void clearBatch() throws CGException;

    abstract public Integer getMaxRows() throws CGException;

    abstract public void setMaxRows(final Integer max) throws CGException;

    abstract public HashMap executeBatch() throws CGException;

    abstract public Integer getFetchSize() throws CGException;

    abstract public JavaSqlResultSet getResultSet() throws CGException;

    abstract public void clearWarnings() throws CGException;

    abstract public JavaSqlConnection getConnection() throws CGException;

    abstract public void setFetchSize(final Integer rows) throws CGException;

    abstract public Boolean getMoreResults() throws CGException;

    abstract public Integer getUpdateCount() throws CGException;

    abstract public Boolean execute(final JavaLangString sql) throws CGException;

    abstract public Integer getMaxFieldSize() throws CGException;

    abstract public Integer getQueryTimeout() throws CGException;

    abstract public void addBatch(final JavaLangString sql) throws CGException;

    abstract public Integer getResultSetType() throws CGException;

    abstract public void setMaxFieldSize(final Integer max) throws CGException;

    abstract public void setQueryTimeout(final Integer seconds) throws CGException;

    abstract public Integer getFetchDirection() throws CGException;

    abstract public void setFetchDirection(final Integer direction) throws CGException;

    abstract public JavaSqlResultSet executeQuery(final JavaLangString sql) throws CGException;

    abstract public void setEscapeProcessing(final Boolean enable) throws CGException;

    abstract public Integer executeUpdate(final JavaLangString sql) throws CGException;

    abstract public void setCursorName(final JavaLangString name) throws CGException;

    abstract public Integer getResultSetConcurrency() throws CGException;
}

