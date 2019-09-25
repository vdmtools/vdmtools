package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;
import jp.vdmtools.VDM.VDMSpecException;

import java.util.HashMap;

public class JavaIoPrintWriter extends JavaIoWriter {

    static UTIL.VDMCompare vdmComp = new UTIL.VDMCompare();

    protected JavaIoWriter out = null;

    private Boolean autoFlush = null;

    private Boolean trouble = null;

    private JavaLangString lineSeparator = null;


    public JavaIoPrintWriter() throws CGException {
        autoFlush = Boolean.FALSE;
        trouble = Boolean.FALSE;
    }


    public void close() throws CGException {
        Boolean cond_1 = null;
        cond_1 = out.IsNil();
        if (cond_1.booleanValue())
            return;
        try {
            out.close();
            out = (JavaIoWriter) new JavaLangObject(new quotes.NIL());
        } catch (VDMSpecException exc_2) {
            if (exc_2.getValue() instanceof JavaLangException) {
                JavaLangException j2v_uexception = (JavaLangException) exc_2.getValue();
                boolean succ_3 = true;
                {
                    succ_3 = true;
                    Boolean tmpVal_v_7 = null;
                    {
                        if ((tmpVal_v_7 = Boolean.valueOf(j2v_uexception instanceof JavaIoIOException)).booleanValue()) {
                            Boolean var2_10 = null;
                            Boolean unArg_11 = null;
                            unArg_11 = j2v_uexception.IsNil();
                            var2_10 = Boolean.valueOf(!unArg_11.booleanValue());
                            tmpVal_v_7 = var2_10;
                        }
                    }
                    if (!UTIL.equals(Boolean.TRUE, tmpVal_v_7))
                        succ_3 = false;
                    if (succ_3)
                        trouble = Boolean.TRUE;
                    else
                        throw new VDMSpecException(j2v_uexception);
                }
            } else {
                UTIL.RunTime("Run-Time Error:Pattern match failed on exception value");
                return;
            }
        }
    }


    public void flush() throws CGException {
        try {
            ensureOpen();
            out.flush();
        } catch (VDMSpecException exc_1) {
            if (exc_1.getValue() instanceof JavaLangException) {
                JavaLangException j2v_uexception = (JavaLangException) exc_1.getValue();
                boolean succ_2 = true;
                {
                    succ_2 = true;
                    Boolean tmpVal_v_6 = null;
                    {
                        if ((tmpVal_v_6 = Boolean.valueOf(j2v_uexception instanceof JavaIoIOException)).booleanValue()) {
                            Boolean var2_9 = null;
                            Boolean unArg_10 = null;
                            unArg_10 = j2v_uexception.IsNil();
                            var2_9 = Boolean.valueOf(!unArg_10.booleanValue());
                            tmpVal_v_6 = var2_9;
                        }
                    }
                    if (!UTIL.equals(Boolean.TRUE, tmpVal_v_6))
                        succ_2 = false;
                    if (succ_2)
                        trouble = Boolean.TRUE;
                    else
                        throw new VDMSpecException(j2v_uexception);
                }
            } else {
                UTIL.RunTime("Run-Time Error:Pattern match failed on exception value");
                return;
            }
        }
    }


    public void write(final Object c) throws CGException {
        try {
            ensureOpen();
            out.write(c);
        } catch (VDMSpecException exc_2) {
            if (exc_2.getValue() instanceof JavaLangException) {
                JavaLangException j2v_uexception = (JavaLangException) exc_2.getValue();
                boolean succ_3 = true;
                {
                    succ_3 = true;
                    Boolean tmpVal_v_7 = null;
                    {
                        if ((tmpVal_v_7 = Boolean.valueOf(j2v_uexception instanceof JavaIoIOException)).booleanValue()) {
                            Boolean var2_10 = null;
                            Boolean unArg_11 = null;
                            unArg_11 = j2v_uexception.IsNil();
                            var2_10 = Boolean.valueOf(!unArg_11.booleanValue());
                            tmpVal_v_7 = var2_10;
                        }
                    }
                    if (!UTIL.equals(Boolean.TRUE, tmpVal_v_7))
                        succ_3 = false;
                    if (succ_3)
                        trouble = Boolean.TRUE;
                    else
                        throw new VDMSpecException(j2v_uexception);
                }
            } else {
                UTIL.RunTime("Run-Time Error:Pattern match failed on exception value");
                return;
            }
        }
    }


    public void println() throws CGException {
        newLine();
    }


    public void println(final Object x) throws CGException {
        print(x);
        println();
    }


    public void print(final Object b) throws CGException {
        boolean succ_2 = true;
        {
            succ_2 = true;
            if (!UTIL.equals(Boolean.TRUE, Boolean.valueOf((b instanceof Boolean))))
                succ_2 = false;
            if (succ_2) {
                JavaLangString tmpArg_v_51 = null;
                String arg_52 = null;
                if (((Boolean) b).booleanValue())
                    arg_52 = new String("true");
                else
                    arg_52 = new String("false");
                tmpArg_v_51 = new JavaLangString(arg_52);
                write((JavaLangString) tmpArg_v_51);
            } else {
                succ_2 = true;
                if (!UTIL.equals(Boolean.TRUE, Boolean.valueOf((b instanceof Character))))
                    succ_2 = false;
                if (succ_2) {
                    Character c = (Character) b;
                    JavaLangString tmpArg_v_43 = null;
                    tmpArg_v_43 = (JavaLangString) JavaLangString.valueOf(c);
                    write((JavaLangString) tmpArg_v_43);
                } else {
                    succ_2 = true;
                    if (!UTIL.equals(Boolean.TRUE, Boolean.valueOf(UTIL.IsInteger(b))))
                        succ_2 = false;
                    if (succ_2) {
                        Integer i = UTIL.NumberToInt(b);
                        JavaLangString tmpArg_v_35 = new JavaLangInteger(i).vdm_toString();
                        write((JavaLangString) tmpArg_v_35);
                    } else {
                        succ_2 = true;
                        if (!UTIL.equals(Boolean.TRUE, Boolean.valueOf(UTIL.IsReal(b))))
                            succ_2 = false;
                        if (succ_2) {
                            Double r = UTIL.NumberToReal(b);
                            JavaLangString tmpArg_v_27 = null;
                            tmpArg_v_27 = (JavaLangString) JavaLangString.valueOf(r);
                            write((JavaLangString) tmpArg_v_27);
                        } else {
                            succ_2 = true;
                            Boolean tmpVal_v_7 = null;
                            tmpVal_v_7 = Boolean.valueOf(b instanceof JavaLangObject);
                            if (!UTIL.equals(Boolean.TRUE, tmpVal_v_7))
                                succ_2 = false;
                            if (succ_2) {
                                Boolean cond_9 = null;
                                cond_9 = Boolean.valueOf(b instanceof JavaLangString);
                                if (cond_9.booleanValue()) {
                                    JavaLangString s = (JavaLangString) b;
                                    Boolean cond_19 = null;
                                    cond_19 = s.IsNil();
                                    if (cond_19.booleanValue())
                                        s = (JavaLangString) UTIL.clone(new JavaLangString(new String("null")));
                                    write((JavaLangString) s);
                                } else {
                                    Object tmpVal_12 = null;
                                    tmpVal_12 = b;
                                    JavaLangObject o = null;
                                    o = (JavaLangObject) tmpVal_12;
                                    JavaLangString tmpArg_v_14 = null;
                                    tmpArg_v_14 = (JavaLangString) JavaLangString.valueOf(o);
                                    write((JavaLangString) tmpArg_v_14);
                                }
                            } else {
                                HashMap m = (HashMap) b;
                                write(m);
                            }
                        }
                    }
                }
            }
        }
    }


    protected void setError() throws CGException {
        trouble = Boolean.TRUE;
    }


    public Boolean checkError() throws CGException {
        Boolean cond_1 = null;
        Boolean unArg_2 = null;
        unArg_2 = out.IsNil();
        cond_1 = Boolean.valueOf(!unArg_2.booleanValue());
        if (cond_1.booleanValue())
            flush();
        return trouble;
    }


    public void write(final JavaLangString s) throws CGException {
        Integer tmpArg_v_5 = null;
        tmpArg_v_5 = s.length();
        write((JavaLangString) s, Integer.valueOf(0), tmpArg_v_5);
    }

    public void writeICMIntInt(final HashMap buf, final Integer off, final Integer len_u_u) throws CGException {
        write(buf, off, len_u_u);
    }

    public void write(final HashMap buf, final Integer off, final Integer len_u_u) throws CGException {
        try {
            ensureOpen();
            out.writeICMIntInt(buf, off, len_u_u);
        } catch (VDMSpecException exc_4) {
            if (exc_4.getValue() instanceof JavaLangException) {
                JavaLangException j2v_uexception = (JavaLangException) exc_4.getValue();
                boolean succ_5 = true;
                {
                    succ_5 = true;
                    Boolean tmpVal_v_9 = null;
                    {
                        if ((tmpVal_v_9 = Boolean.valueOf(j2v_uexception instanceof JavaIoIOException)).booleanValue()) {
                            Boolean var2_12 = null;
                            Boolean unArg_13 = null;
                            unArg_13 = j2v_uexception.IsNil();
                            var2_12 = Boolean.valueOf(!unArg_13.booleanValue());
                            tmpVal_v_9 = var2_12;
                        }
                    }
                    if (!UTIL.equals(Boolean.TRUE, tmpVal_v_9))
                        succ_5 = false;
                    if (succ_5)
                        trouble = Boolean.TRUE;
                    else
                        throw new VDMSpecException(j2v_uexception);
                }
            } else {
                UTIL.RunTime("Run-Time Error:Pattern match failed on exception value");
                return;
            }
        }
    }


    public JavaIoPrintWriter(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = Boolean.TRUE;
    }


    public void write(final JavaLangString s, final Integer off, final Integer len_u_u) throws CGException {
        try {
            ensureOpen();
            Integer par_19 = null;
            par_19 = s.length();
            out.writeStrIntInt((JavaLangString) s, Integer.valueOf(0), par_19);
        } catch (VDMSpecException exc_4) {
            if (exc_4.getValue() instanceof JavaLangException) {
                JavaLangException j2v_uexception = (JavaLangException) exc_4.getValue();
                boolean succ_5 = true;
                {
                    succ_5 = true;
                    Boolean tmpVal_v_9 = null;
                    {
                        if ((tmpVal_v_9 = Boolean.valueOf(j2v_uexception instanceof JavaIoIOException)).booleanValue()) {
                            Boolean var2_12 = null;
                            Boolean unArg_13 = null;
                            unArg_13 = j2v_uexception.IsNil();
                            var2_12 = Boolean.valueOf(!unArg_13.booleanValue());
                            tmpVal_v_9 = var2_12;
                        }
                    }
                    if (!UTIL.equals(Boolean.TRUE, tmpVal_v_9))
                        succ_5 = false;
                    if (succ_5)
                        trouble = Boolean.TRUE;
                    else
                        throw new VDMSpecException(j2v_uexception);
                }
            } else {
                UTIL.RunTime("Run-Time Error:Pattern match failed on exception value");
                return;
            }
        }
    }


    private void newLine() throws CGException {
        try {
            ensureOpen();
            out.writeStr((JavaLangString) lineSeparator);
            out.flush();
        } catch (VDMSpecException exc_1) {
            if (exc_1.getValue() instanceof JavaLangException) {
                JavaLangException j2v_uexception = (JavaLangException) exc_1.getValue();
                boolean succ_2 = true;
                {
                    succ_2 = true;
                    Boolean tmpVal_v_6 = null;
                    {
                        if ((tmpVal_v_6 = Boolean.valueOf(j2v_uexception instanceof JavaIoIOException)).booleanValue()) {
                            Boolean var2_9 = null;
                            Boolean unArg_10 = null;
                            unArg_10 = j2v_uexception.IsNil();
                            var2_9 = Boolean.valueOf(!unArg_10.booleanValue());
                            tmpVal_v_6 = var2_9;
                        }
                    }
                    if (!UTIL.equals(Boolean.TRUE, tmpVal_v_6))
                        succ_2 = false;
                    if (succ_2)
                        trouble = Boolean.TRUE;
                    else
                        throw new VDMSpecException(j2v_uexception);
                }
            } else {
                UTIL.RunTime("Run-Time Error:Pattern match failed on exception value");
                return;
            }
        }
    }


    public JavaIoPrintWriter(final JavaIoStringWriter out_1) throws CGException {
        this((JavaIoWriter) out_1, Boolean.FALSE);
    }


    public JavaIoPrintWriter(final JavaIoWriter out_1, final Boolean autoFlush_1) throws CGException {
        super(out_1);
        ((JavaIoPrintWriter) this).out = (JavaIoWriter) UTIL.clone(out_1);
        ((JavaIoPrintWriter) this).autoFlush = (Boolean) UTIL.clone(autoFlush_1);
        lineSeparator = (JavaLangString) UTIL.clone(new JavaLangString(new String("\n")));
    }


    public JavaIoPrintWriter(final JavaIoOutputStream out_1) throws CGException {
        this(out_1, Boolean.FALSE);
    }


    public JavaIoPrintWriter(final JavaIoOutputStream out_1, final Boolean autoFlush_1) throws CGException {
        this(new JavaIoBufferedWriter(new JavaIoOutputStreamWriter(out_1)),
	     Boolean.FALSE);
    }


    private void ensureOpen() throws CGException {
        Boolean cond_1 = null;
        cond_1 = out.IsNil();
        if (cond_1.booleanValue())
            throw new VDMSpecException(new JavaIoIOException(new JavaLangString(new String("Stream closed"))));
    }
}

