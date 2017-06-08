package jp.vdmtools.VDM.jdk;


import jp.vdmtools.VDM.UTIL;
import jp.vdmtools.VDM.CGException;

import java.util.HashMap;


public class JavaLangStringBuffer extends JavaLangObject {

    public static final JavaLangClass CLASS;

    private StringBuffer wrappedObject;


    static {
        CLASS = new JavaLangClass(new String("JavaLangStringBuffer"),
                new quotes.CLASS(),
                new Boolean(false));
    }


    public JavaLangStringBuffer() {
        wrappedObject = new StringBuffer();
    }


    public JavaLangStringBuffer(final quotes.NIL p) {
        wrappedObject = null;
        isNil = (Boolean) UTIL.clone(new Boolean(true));
    }


//    public JavaLangClass getClass() {
//        return CLASS;
//    }


    public JavaLangStringBuffer(final Object arg) {
        if (arg instanceof Number)
            wrappedObject = new StringBuffer(((Number) arg).intValue());
        else if (arg instanceof JavaLangString)
	    wrappedObject = new StringBuffer(((JavaLangString) arg).toSeqOfChar());
	else
            wrappedObject = new StringBuffer((String) arg);
    }


    public JavaLangStringBuffer(final Character arg) {
        wrappedObject = new StringBuffer(arg.toString());
    }


    public JavaLangStringBuffer append(final Object anything) {
        if (anything instanceof HashMap) {
            HashMap hm = (HashMap) anything;
            char chars[] = new char[hm.size()];
            for (int i = 0; i < hm.size(); i++)
                chars[i] = ((Character) hm.get(new Integer(i))).charValue();
            wrappedObject.append(chars);
        } 
	else if (anything instanceof JavaLangString)
	    wrappedObject.append(((JavaLangString) anything).toSeqOfChar());
	else if (anything instanceof JavaLangObject){
	    try {
		wrappedObject.append(((JavaLangObject) anything).vdm_toString().toSeqOfChar());
	    } catch (CGException e) {
		System.err.println("Exception in JavaLangStringBuffer.append");
		e.printStackTrace(System.err);
	    }
	}	    
	else
            wrappedObject.append(anything);
        return this;
    }


    public Integer capacity() {
        return new Integer(wrappedObject.capacity());
    }


    public Character charAt(final Integer charIndex) {
        return new Character(wrappedObject.charAt(charIndex.intValue()));
    }


    public JavaLangStringBuffer delete(final Integer beginIndex,
                                       final Integer endIndex) {
        wrappedObject.delete(beginIndex.intValue(),
                endIndex.intValue());
        return this;
    }


    public JavaLangStringBuffer deleteCharAt(final Integer charIndex) {
        wrappedObject.deleteCharAt(charIndex.intValue());
        return this;
    }


    public void ensureCapacity(final Integer newLength) {
        wrappedObject.ensureCapacity(newLength.intValue());
    }


    public JavaLangStringBuffer insert(final Integer offset,
                                       final Object anything) {
        if (anything instanceof HashMap) {
            HashMap hm = (HashMap) anything;
            char chars[] = new char[hm.size()];
            for (int i = 0; i < hm.size(); i++)
                chars[i] = ((Character) hm.get(new Integer(i))).charValue();
            wrappedObject.insert(offset.intValue(), chars);
        } else
            wrappedObject.insert(offset.intValue(), anything);
        return this;
    }


    public Integer length() {
        return new Integer(wrappedObject.length());
    }


    public JavaLangStringBuffer replace(final Integer beginIndex,
                                        final Integer endIndex,
                                        final Object aString) {
        if (aString instanceof JavaLangString)
            wrappedObject.replace(beginIndex.intValue(),
                    endIndex.intValue(),
                    ((JavaLangString) aString).toSeqOfChar());
        else
            wrappedObject.replace(beginIndex.intValue(),
                    endIndex.intValue(),
                    (String) aString);
        return this;
    }


    public JavaLangStringBuffer reverse_q() {
        wrappedObject.reverse();
        return this;
    }


    //  public JavaLangStringBuffer substring (final Integer beginIndex,
    //					 final Integer endIndex) {
    public JavaLangString substring(final Integer beginIndex,
                                    final Integer endIndex) {
        return new JavaLangString(wrappedObject.substring(
                beginIndex.intValue(),
                endIndex.intValue()));
    }


   public JavaLangString vdm_toString() {
       return new JavaLangString(wrappedObject.toString());
   }


    public String toSeqOfChar() {
        return wrappedObject.toString();
    }
}
