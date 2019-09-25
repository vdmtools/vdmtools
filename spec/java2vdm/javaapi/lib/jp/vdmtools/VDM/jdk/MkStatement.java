/*
 * Created by IntelliJ IDEA.
 * User: paulm
 * Date: Nov 22, 2001
 * Time: 12:32:14 PM
 * To change template for new class use
 * Code Style | Class Templates options (Tools | IDE Options).
 */
package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.VDMSpecException;

import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashMap;


public class MkStatement extends JavaLangObject implements JavaSqlStatement {
    private Statement wrappedObject;

    public Boolean IsNil(){
        return isNil;
    }

    public MkStatement(Statement st) throws CGException {
        wrappedObject = st;

    }

    public void close() throws CGException {
        try {
            wrappedObject.close();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void cancel() throws CGException {
        try {
            wrappedObject.cancel();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void clearBatch() throws CGException {
        try {
            wrappedObject.clearBatch();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getMaxRows() throws CGException {
        try {
            return Integer.valueOf(wrappedObject.getMaxRows());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void setMaxRows(final Integer max) throws CGException {
        try {
            wrappedObject.setMaxRows(max.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public HashMap executeBatch() throws CGException {
        try {
            int[] vals = wrappedObject.executeBatch();
            HashMap result = new HashMap();
            for (int i = 0; i < vals.length; i++)
                result.put(Integer.valueOf(i), Integer.valueOf(vals[i]));
            return result;
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getFetchSize() throws CGException {
        try {
            return Integer.valueOf(wrappedObject.getFetchSize());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public JavaSqlResultSet getResultSet() throws CGException {
        try {
            return new MkResultSet(wrappedObject.getResultSet());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void clearWarnings() throws CGException {
        try {
            wrappedObject.clearWarnings();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public JavaSqlConnection getConnection() throws CGException {
        try {
            return new MkConnection(wrappedObject.getConnection());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void setFetchSize(final Integer rows) throws CGException {
        try {
            wrappedObject.setFetchSize(rows.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean getMoreResults() throws CGException {
        try {
            return Boolean.valueOf(wrappedObject.getMoreResults());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getUpdateCount() throws CGException {
        try {
            return Integer.valueOf(wrappedObject.getUpdateCount());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Boolean execute(final JavaLangString sql) throws CGException {
        try {
            return Boolean.valueOf(wrappedObject.execute(sql.toSeqOfChar()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getMaxFieldSize() throws CGException {
        try {
            return Integer.valueOf(wrappedObject.getMaxFieldSize());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getQueryTimeout() throws CGException {
        try {
            return Integer.valueOf(wrappedObject.getQueryTimeout());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void addBatch(final JavaLangString sql) throws CGException {
        try {
            wrappedObject.addBatch(sql.toSeqOfChar());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getResultSetType() throws CGException {
        try {
            return Integer.valueOf(wrappedObject.getResultSetType());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void setMaxFieldSize(final Integer max) throws CGException {
        try {
            wrappedObject.setMaxFieldSize(max.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void setQueryTimeout(final Integer seconds) throws CGException {
        try {
            wrappedObject.setQueryTimeout(seconds.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getFetchDirection() throws CGException {
        try {
            return Integer.valueOf(wrappedObject.getFetchDirection());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void setFetchDirection(final Integer direction) throws CGException {
        try {
            wrappedObject.setFetchDirection(direction.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public JavaSqlResultSet executeQuery(final JavaLangString sql) throws CGException {
        try {
            return new MkResultSet(wrappedObject.executeQuery(sql.toSeqOfChar()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void setEscapeProcessing(final Boolean enable) throws CGException {
        try {
            wrappedObject.setEscapeProcessing(enable.booleanValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer executeUpdate(final JavaLangString sql) throws CGException {
        try {
            return Integer.valueOf(wrappedObject.executeUpdate(sql.toSeqOfChar()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void setCursorName(final JavaLangString name) throws CGException {
        try {
            wrappedObject.setCursorName(name.toSeqOfChar());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public Integer getResultSetConcurrency() throws CGException {
        try {
            return Integer.valueOf(wrappedObject.getResultSetConcurrency());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }
}
