package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;


public interface JavaSqlConnection extends Nullable {


    public static final Integer TRANSACTION_uNONE = new Integer(0);
    public static final Integer TRANSACTION_uREAD_uUNCOMMITTED = new Integer(0);
    public static final Integer TRANSACTION_uREAD_uCOMMITTED = new Integer(0);
    public static final Integer TRANSACTION_uREPEATABLE_uREAD = new Integer(0);
    public static final Integer TRANSACTION_uSERIALIZABLE = new Integer(0);


    abstract public void close() throws CGException ;

    abstract public void commit()throws CGException ;

    abstract public Boolean isClosed()throws CGException ;

    abstract public void rollback()throws CGException ;

    abstract public JavaLangString getCatalog()throws CGException ;

    abstract public JavaUtilMap getTypeMap()throws CGException ;

    abstract public Boolean isReadOnly()throws CGException ;

    abstract public void setReadOnly(final Boolean readOnly)throws CGException ;

    abstract public void clearWarnings()throws CGException ;

    abstract public Boolean getAutoCommit()throws CGException ;

    abstract public void setAutoCommit(final Boolean autoCommit)throws CGException ;

    abstract public JavaSqlStatement createStatement()throws CGException ;

    abstract public JavaLangString nativeSQL(final JavaLangString sql)throws CGException ;

    abstract public JavaSqlStatement createStatement(final Integer resultSetType,
                                                     final Integer resultSetConcurrency)throws CGException ;

    abstract public void setCatalog(final JavaLangString catalog)throws CGException ;

    abstract public Integer getTransactionIsolation()throws CGException ;

    abstract public void setTransactionIsolation(final Integer level)throws CGException ;

}
