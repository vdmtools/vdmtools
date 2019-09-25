/*
 * Created by IntelliJ IDEA.
 * User: paulm
 * Date: Nov 22, 2001
 * Time: 12:32:58 PM
 * To change template for new class use
 * Code Style | Class Templates options (Tools | IDE Options).
 */
package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.VDMSpecException;
import jp.vdmtools.VDM.CGException;

import java.sql.Connection;
import java.sql.SQLException;

public class MkConnection extends JavaLangObject implements JavaSqlConnection {
    private Connection wrappedObject;

    public MkConnection (Connection conn) {
        wrappedObject = conn;
    }

    public Boolean IsNil() {
        return isNil;
    }

    public void close() throws CGException {
        try {
            wrappedObject.close();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }
    public void commit() throws CGException {
        try {
            wrappedObject.commit();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }
    public Boolean isClosed() throws CGException {
        try {
            return Boolean.valueOf(wrappedObject.isClosed());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }
    public void rollback() throws CGException {
        try {
            wrappedObject.rollback();
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }
    public JavaLangString getCatalog() throws CGException {
        try {
            return new JavaLangString(wrappedObject.getCatalog());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }
    public JavaUtilMap getTypeMap() throws CGException {
        return null;
    }

    public Boolean isReadOnly() throws CGException {
        try {
            return Boolean.valueOf(wrappedObject.isReadOnly());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void setReadOnly(final Boolean readOnly) throws CGException {
        try {
            wrappedObject.setReadOnly(readOnly.booleanValue());
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
    public Boolean getAutoCommit() throws CGException {
        try {
            return Boolean.valueOf(wrappedObject.getAutoCommit());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }
    public void setAutoCommit(final Boolean autoCommit) throws CGException {
        try {
            wrappedObject.setAutoCommit(autoCommit.booleanValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }
    public JavaSqlStatement createStatement() throws CGException {
        try {
            return new MkStatement(wrappedObject.createStatement());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }
    public JavaLangString nativeSQL(final JavaLangString sql) throws CGException {
        try {
            return new JavaLangString(wrappedObject.nativeSQL(sql.toSeqOfChar()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }
    public JavaSqlStatement createStatement(final Integer resultSetType,
                                            final Integer resultSetConcurrency) throws CGException {
        try {
            return new MkStatement(wrappedObject.createStatement(resultSetType.intValue(),
                                   resultSetConcurrency.intValue()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }

    public void setCatalog(final JavaLangString catalog) throws CGException {
        try {
            wrappedObject.setCatalog(catalog.toSeqOfChar());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }
    public Integer getTransactionIsolation() throws CGException {
        try {
            return Integer.valueOf(wrappedObject.getTransactionIsolation());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }
    public void setTransactionIsolation(final Integer level) throws CGException {
        try {
            wrappedObject.setTransactionIsolation(level.intValue());
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }
}
