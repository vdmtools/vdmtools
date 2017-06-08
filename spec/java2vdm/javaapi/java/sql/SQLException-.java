package java.sql;

public class SQLException extends Exception {
//public SQLException( String reason,  String SQLState,  int vendorCode){};
//public SQLException( String reason,  String SQLState){};
//public SQLException( String reason){};
//public SQLException(){};
//public native String getSQLState();
//public native int getErrorCode();
public native SQLException getNextException();
//public synchronized native void setNextException( SQLException ex);

}
