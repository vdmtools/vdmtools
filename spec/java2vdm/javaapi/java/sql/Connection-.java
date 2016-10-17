package java.sql;


public interface Connection {
Statement createStatement()throws SQLException;
//PreparedStatement prepareStatement( String sql)throws SQLException;
//CallableStatement prepareCall( String sql)throws SQLException;
//String nativeSQL( String sql)throws SQLException;
//void setAutoCommit( boolean autoCommit)throws SQLException;
//boolean getAutoCommit()throws SQLException;
//void commit()throws SQLException;
//void rollback()throws SQLException;
void close()throws SQLException;
//boolean isClosed()throws SQLException;
//DatabaseMetaData getMetaData()throws SQLException;
//void setReadOnly( boolean readOnly)throws SQLException;
//boolean isReadOnly()throws SQLException;
//void setCatalog( String catalog)throws SQLException;
//String getCatalog()throws SQLException;
// int TRANSACTION_NONE;
// int TRANSACTION_READ_UNCOMMITTED;
// int TRANSACTION_READ_COMMITTED;
// int TRANSACTION_REPEATABLE_READ;
// int TRANSACTION_SERIALIZABLE;
//void setTransactionIsolation( int level)throws SQLException;
//int getTransactionIsolation()throws SQLException;
//SQLWarning getWarnings()throws SQLException;
//void clearWarnings()throws SQLException;
Statement createStatement( int resultSetType,  int resultSetConcurrency)throws SQLException;
//PreparedStatement prepareStatement( String sql,  int resultSetType,  int resultSetConcurrency)throws SQLException;
//CallableStatement prepareCall( String sql,  int resultSetType,  int resultSetConcurrency)throws SQLException;
//java.util.Map getTypeMap()throws SQLException;
//void setTypeMap( java.util.Map map)throws SQLException;
 }
