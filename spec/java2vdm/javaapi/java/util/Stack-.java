package java.util;

public class Stack extends Vector {
public Stack(){};
public native Object push( Object item);
public synchronized native Object pop();
//public synchronized native Object peek();
public native boolean empty();
//public synchronized native int search( Object o);

}
