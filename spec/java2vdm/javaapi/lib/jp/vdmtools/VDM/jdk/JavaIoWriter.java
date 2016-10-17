

package jp.vdmtools.VDM.jdk;


import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;

import java.util.HashMap;
import java.util.HashSet;


public abstract class JavaIoWriter extends JavaLangObject {

    static UTIL.VDMCompare vdmComp = new UTIL.VDMCompare();

    private HashMap writeBuffer = new HashMap();

    private Integer writeBufferSize = null;

    private static final JavaLangClass CLASS = (JavaLangClass) new JavaLangClass(new String("JavaIoWriter"), new quotes.CLASS(), new Boolean(false));


    public JavaIoWriter(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = (Boolean) UTIL.clone(new Boolean(true));
    }


    public abstract void close() throws CGException;


    public abstract void flush() throws CGException;


    protected JavaIoWriter() throws CGException {
        ;
    }


    public void write(final Object a) throws CGException {
        boolean succ_2 = true;
        {
            succ_2 = true;
            if (!UTIL.equals(new Boolean(true), new Boolean(UTIL.IsInteger(a))))
                succ_2 = false;
            if (succ_2)
                writeInt(UTIL.NumberToInt(a));
            else
                writeMIC((HashMap) a);
        }
    }


    public void writeInt(final Integer c) throws CGException {
        HashMap tmpArg_v_3 = new HashMap();
        Character tmpVar2_5 = null;
        JavaLangInteger obj_6 = null;
        obj_6 = (JavaLangInteger) new JavaLangInteger(c);
        tmpVar2_5 = obj_6.charValue();
        tmpArg_v_3 = new HashMap();
        tmpArg_v_3.put(new Integer(0), tmpVar2_5);
        writeICMIntInt(tmpArg_v_3, new Integer(0), new Integer(1));
    }


    public JavaLangClass getJClass() throws CGException {
        return (JavaLangClass) CLASS;
    }


    public void writeMIC(final HashMap cbuf) throws CGException {
        Integer tmpArg_v_5 = null;
        HashSet unArg_6 = new HashSet();
        unArg_6.clear();
        unArg_6.addAll(cbuf.keySet());
        tmpArg_v_5 = new Integer(unArg_6.size());
        writeICMIntInt(cbuf, new Integer(0), tmpArg_v_5);
    }


    public abstract void writeICMIntInt(final HashMap cbuf, final Integer off, final Integer len_u_u) throws CGException;


    public void writeStr(final JavaLangString str) throws CGException {
        Integer tmpArg_v_5 = null;
        tmpArg_v_5 = str.length();
        writeStrIntInt((JavaLangString) str, new Integer(0), tmpArg_v_5);
    }


    protected JavaIoWriter(final JavaLangObject lock) throws CGException {
        this();
        ;
    }


    public void writeStrIntInt(final JavaLangString str, final Integer off, final Integer len_u_u) throws CGException {
        {
            HashMap cbuf = new HashMap();
            HashMap rhs_4 = new HashMap();
            JavaLangString obj_5 = str.substring(off, 
						 new Integer(off.intValue() +
							     len_u_u.intValue()));
            rhs_4 = obj_5.toCharArray();
            cbuf = (HashMap) UTIL.clone(rhs_4);
            writeICMIntInt(cbuf, new Integer(0), len_u_u);
        }
        ;
    }

}

;
