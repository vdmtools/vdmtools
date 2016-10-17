package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.VDMSpecException;

import java.sql.DriverManager;
import java.sql.Connection;
import java.sql.Statement;
import java.sql.SQLException;
import java.util.HashMap;

public class JavaSqlDriverManager extends JavaLangObject {

    private static final JavaLangClass CLASS;
    private DriverManager wrappedObject;

    static {
        CLASS = new JavaLangClass(new String("JavaSqlDriverManager"),
                new quotes.CLASS(),
                new Boolean(false));
    }


    private JavaSqlDriverManager(final quotes.NIL v) {
        wrappedObject = null;
        isNil = new Boolean(true);
    }


    public JavaLangClass getJClass() {
        return CLASS;
    }



    static public JavaSqlConnection getConnection(final JavaLangString url) throws CGException {
        try {
            return new MkConnection(DriverManager.getConnection(url.toSeqOfChar()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }
    }


    static public JavaSqlConnection getConnection(final JavaLangString url,
                                                  final JavaLangString user,
                                                  final JavaLangString password) throws CGException {
        try {
            return new MkConnection(
                    DriverManager.getConnection(url.toSeqOfChar(),
                            user.toSeqOfChar(),
                            password.toSeqOfChar()));
        } catch (SQLException e) {
            throw new VDMSpecException(new JavaSqlSQLException(e));
        }

    }


}
