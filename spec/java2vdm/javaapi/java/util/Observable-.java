package java.util;

public class Observable {
//public Observable(){};
//public synchronized native void addObserver( Observer o);
//public synchronized native void deleteObserver( Observer o);
//public native void notifyObservers();
public native void notifyObservers( Object arg);
//public synchronized native void deleteObservers();
protected synchronized native void setChanged();
protected synchronized native void clearChanged();
//public synchronized native boolean hasChanged();
//public synchronized native int countObservers();

}
