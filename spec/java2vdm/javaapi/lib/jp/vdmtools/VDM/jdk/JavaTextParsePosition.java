package jp.vdmtools.VDM.jdk;


import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;


public class JavaTextParsePosition extends JavaLangObject {

    public Integer index = null;
    public Integer errorIndex = null;
    private static final JavaLangClass CLASS = (JavaLangClass) new JavaLangClass(new String("JavaTextParsePosition"), new quotes.CLASS(), Boolean.FALSE);

    public JavaTextParsePosition() throws CGException {
        try {
            index = Integer.valueOf(0);
            errorIndex = Integer.valueOf(-Integer.valueOf(1).intValue());
        } catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }

    public JavaLangClass getJClass() throws CGException {
        return (JavaLangClass) CLASS;
    }

    public Integer getIndex() throws CGException {
        return index;
    }

    public JavaLangString toJString() throws CGException {
        JavaLangString rexpr_1 = null;
        JavaLangString par_2 = null;
        JavaLangString par_3 = null;
        JavaLangString par_4 = null;
        JavaLangString par_5 = null;
        JavaLangString par_6 = null;
        JavaLangClass obj_7 = null;
        obj_7 = (JavaLangClass) getJClass();
        par_6 = (JavaLangString) obj_7.getName();
        par_5 = (JavaLangString) J2VUTIL.ConcatStr((JavaLangString) par_6, (JavaLangString) new JavaLangString(new String("[index=")));
        Object par_10 = null;
        par_10 = J2VUTIL.Convert(new String("JavaLangString"), index);
        par_4 = (JavaLangString) J2VUTIL.ConcatStr((JavaLangString) par_5, (JavaLangString) par_10);
        par_3 = (JavaLangString) J2VUTIL.ConcatStr((JavaLangString) par_4, (JavaLangString) new JavaLangString(new String(",errorIndex=")));
        Object par_15 = null;
        par_15 = J2VUTIL.Convert(new String("JavaLangString"), errorIndex);
        par_2 = (JavaLangString) J2VUTIL.ConcatStr((JavaLangString) par_3, (JavaLangString) par_15);
        Object par_18 = null;
        par_18 = J2VUTIL.Convert(new String("JavaLangString"), Character.valueOf(']'));
        rexpr_1 = (JavaLangString) J2VUTIL.ConcatStr((JavaLangString) par_2, (JavaLangString) par_18);
        return (JavaLangString) rexpr_1;
    }

    public void setIndex(final Integer index_1) throws CGException {
        ((JavaTextParsePosition) this).index = index_1;
        ;
    }

    public Integer getErrorIndex() throws CGException {
        return errorIndex;
    }

    public JavaTextParsePosition(final Integer index_1) throws CGException {
        this();
        ((JavaTextParsePosition) this).index = index_1;
    }

    public Boolean equals(final JavaLangObject obj) throws CGException {
        Boolean cond_2 = null;
        cond_2 = obj.IsNil();
        if (cond_2.booleanValue())
            return Boolean.FALSE;
        if (Boolean.valueOf(!Boolean.valueOf(obj instanceof JavaTextParsePosition).booleanValue()).booleanValue())
            return Boolean.FALSE;
        {
            JavaTextParsePosition other = (JavaTextParsePosition) obj;
            Boolean rexpr_8 = null;
            Boolean var1_9 = null;
            Integer var2_11 = null;
            var2_11 = other.index;
            var1_9 = Boolean.valueOf(index.intValue() == var2_11.intValue());
            {
                if ((rexpr_8 = var1_9).booleanValue()) {
                    Boolean var2_13 = null;
                    Integer var2_15 = null;
                    var2_15 = other.errorIndex;
                    var2_13 = Boolean.valueOf(errorIndex.intValue() == var2_15.intValue());
                    rexpr_8 = var2_13;
                }
            }
            return rexpr_8;
        }
    }

    public void setErrorIndex(final Integer ei) throws CGException {
        errorIndex = ei;
    }

    public JavaTextParsePosition(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = Boolean.TRUE;
    }
}


