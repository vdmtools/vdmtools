

package jp.vdmtools.VDM.jdk;


import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.VDMSpecException;
import jp.vdmtools.VDM.Record;
import jp.vdmtools.VDM.UTIL;

import java.util.HashMap;


public class JavaLangClass extends JavaLangObject implements JavaIoSerializable {


    private static class CLASS implements Record {

        public String name;

        public Object cori;

        public Boolean isPrim;


        public CLASS() {
	    name = "undefined";
	    isPrim = new Boolean(false);
	    cori = null;
        }


        public CLASS(String p1, Object p2, Boolean p3) {
            name = p1;
            cori = p2;
            isPrim = p3;
        }


        public Object clone() {
            return new CLASS(name, (Object) UTIL.clone(cori), isPrim);
        }


        public String toString() {
            return "mk_JavaLangClass`CLASS(" + UTIL.toString(name) + "," + UTIL.toString(cori) + "," + UTIL.toString(isPrim) + ")";
        }


        public boolean equals(Object obj) {
            if (!(obj instanceof CLASS))
                return false;
            else {
                CLASS temp = (CLASS) obj;
                return UTIL.equals(name, temp.name) && UTIL.equals(cori, temp.cori) && UTIL.equals(isPrim, temp.isPrim);
            }
        }


        public int hashCode() {
            return (name == null ? 0 : name.hashCode()) + (cori == null ? 0 : cori.hashCode()) + (isPrim == null ? 0 : isPrim.hashCode());
        }

    }
    ;

    static UTIL.VDMCompare vdmComp = new UTIL.VDMCompare();

    private CLASS val = null;


    public JavaLangClass() {
	val = new CLASS();
    }


    public JavaLangClass(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = (Boolean) UTIL.clone(new Boolean(true));
    }


    public JavaLangClass(final String name, final Object cori, final Boolean prim) {
        this();
        {
	    int y = 1;
	    if (name == null) {
		y = 0;
	    }
	    int x = 1/y;
            val.name = name;
            val.isPrim = prim;
            val.cori = cori;
        }
    }


    public JavaLangString getName() throws CGException {
        JavaLangString rexpr_1 = null;
        String arg_2 = null;
        arg_2 = (val).name;
        rexpr_1 = new JavaLangString(arg_2);
        return (JavaLangString) rexpr_1;
    }


    public Boolean isArray() throws CGException {
        return new Boolean(false);
    }


    public JavaLangString toJString() throws CGException {
        String str = null;
        if (isInterface().booleanValue())
            str = new String("interface ");
	//        else if (isPrimitive().booleanValue())
	//            str = new String("");
        else
            str = new String("class ");
        String rhs_5 = null;
        String var2_7 = null;
        JavaLangString obj_8 = null;
        obj_8 = (JavaLangString) getName();
        var2_7 = obj_8.toSeqOfChar();
	if (var2_7 == null) {
	    if (val.name == null)
		System.out.println("name is null");
	    else
		System.out.println("name is " + val.name);
	    System.out.println("getName().toSeqOfChar() is null");
	}
        rhs_5 = str.concat(var2_7);
        str = UTIL.ConvertToString(UTIL.clone(rhs_5));
        return (JavaLangString) new JavaLangString(str);
    }


    public HashMap getClasses() throws CGException {
        return new HashMap();
    }


    public HashMap getSigners() throws CGException {
        return new HashMap();
    }


    public Boolean isInterface() throws CGException {
        Boolean rexpr_1 = null;
        Object var1_2 = null;
        var1_2 = (val).cori;
        rexpr_1 = new Boolean(UTIL.equals(var1_2, new quotes.INTERFACE()));
        return rexpr_1;
    }


    public Boolean isPrimitive() throws CGException {
        Boolean rexpr_1 = null;
        rexpr_1 = (val).isPrim;
        return rexpr_1;
    }


    public JavaLangObject newInstance() throws CGException {
        return (JavaLangObject) new JavaLangObject();
    }


    public HashMap getInterfaces() throws CGException {
        return new HashMap();
    }

    public Integer vdm_hashCode() throws CGException {
	return new Integer(val.hashCode());
    }

    public Boolean vdm_equals(JavaLangObject obj) throws CGException {
	if (!(obj instanceof JavaLangClass))
	    return new Boolean(true);
	
	JavaLangClass cl = (JavaLangClass) obj;
	return new Boolean (cl.toJString().toSeqOfChar().equals(toJString().toSeqOfChar()));
    }


    static public JavaLangClass forName(final JavaLangString className) throws CGException {
	try {
	    Class.forName(className.toSeqOfChar());
	    return new JavaLangClass(className.toSeqOfChar(), 
				     new quotes.CLASS(), 
				     new Boolean(false));
	} catch (ClassNotFoundException e) {
	    throw new VDMSpecException("Class not found: " + 
				       className.toSeqOfChar());
	}
    }
}


