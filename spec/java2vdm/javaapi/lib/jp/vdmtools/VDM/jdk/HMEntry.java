

package jp.vdmtools.VDM.jdk;


import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;


public class HMEntry extends JavaLangObject implements JavaUtilEntry {

    static UTIL.VDMCompare vdmComp = new UTIL.VDMCompare();

    public Integer hash = null;

    public JavaLangObject key = null;

    public JavaLangObject value_u_u = null;

    public HMEntry next = null;


    public HMEntry() throws CGException {
    }


    public HMEntry(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = Boolean.TRUE;
    }


    public HMEntry(final Integer hash_1, final JavaLangObject key_1, final JavaLangObject value_u_u_1, final HMEntry next_1) throws CGException {
        this();
        {
            ((HMEntry) this).hash = UTIL.NumberToInt(UTIL.clone(hash_1));
            ((HMEntry) this).key = (JavaLangObject) UTIL.clone(key_1);
            ((HMEntry) this).value_u_u = (JavaLangObject) UTIL.clone(value_u_u_1);
            ((HMEntry) this).next = (HMEntry) UTIL.clone(next_1);
        }
    }


    public JavaLangObject jClone() throws CGException {
        HMEntry rexpr_1 = null;
        Object arg_5 = null;
        Boolean cond_7 = null;
        cond_7 = next.IsNil();
        if (cond_7.booleanValue())
            arg_5 = new HMEntry(new quotes.NIL());
        else
            arg_5 = next.jClone();
        rexpr_1 = new HMEntry(hash, key, value_u_u, (HMEntry) arg_5);
        return (JavaLangObject) rexpr_1;
    }


    public JavaLangObject getKey() throws CGException {
        return (JavaLangObject) key;
    }


    public JavaLangObject getValue() throws CGException {
        return (JavaLangObject) value_u_u;
    }


    public JavaLangObject setValue(final JavaLangObject value_u_u_1) throws CGException {
        JavaLangObject oldValue = ((HMEntry) this).value_u_u;
        ((HMEntry) this).value_u_u = (JavaLangObject) UTIL.clone(value_u_u_1);
        return (JavaLangObject) oldValue;
    }


    public Boolean equals(final JavaLangObject o) throws CGException {
        if (Boolean.valueOf(!Boolean.valueOf(o instanceof HMEntry).booleanValue()).booleanValue())
            return Boolean.FALSE;
        {
            HMEntry e = (HMEntry) o;
            Boolean rexpr_6 = null;
            Boolean cond_8 = null;
            cond_8 = key.IsNil();
            if (cond_8.booleanValue()) {
                JavaLangObject obj_16 = null;
                obj_16 = (JavaLangObject) e.getKey();
                rexpr_6 = obj_16.IsNil();
            } else {
                Boolean var1_9 = null;
                JavaLangObject par_10 = null;
                par_10 = (JavaLangObject) e.getKey();
                var1_9 = key.vdm_equals((JavaLangObject) par_10);
                {
                    if ((rexpr_6 = var1_9).booleanValue()) {
                        Boolean var2_11 = null;
                        Boolean cond_13 = null;
                        cond_13 = value_u_u.IsNil();
                        if (cond_13.booleanValue()) {
                            JavaLangObject obj_15 = null;
                            obj_15 = (JavaLangObject) e.getValue();
                            var2_11 = obj_15.IsNil();
                        } else {
                            JavaLangObject par_14 = null;
                            par_14 = (JavaLangObject) e.getValue();
                            var2_11 = value_u_u.vdm_equals((JavaLangObject) par_14);
                        }
                        rexpr_6 = var2_11;
                    }
                }
            }
            return rexpr_6;
        }
    }


    public Integer jHashCode() throws CGException {
        Integer rexpr_1 = null;
        Boolean cond_3 = null;
        cond_3 = value_u_u.IsNil();
        if (cond_3.booleanValue())
            rexpr_1 = Integer.valueOf(0);
        else
            rexpr_1 = value_u_u.vdm_hashCode();
        return rexpr_1;
    }


    public JavaLangString toJString() throws CGException {
        JavaLangString s = null;
        s = (JavaLangString) key.vdm_toString();
        JavaLangString rhs_1 = null;
        rhs_1 = (JavaLangString) s.concat((JavaLangString) new JavaLangString(new String("=")));
        s = (JavaLangString) UTIL.clone(rhs_1);
        JavaLangString rhs_4 = null;
        JavaLangString par_5 = null;
        par_5 = (JavaLangString) value_u_u.vdm_toString();
        rhs_4 = (JavaLangString) s.concat((JavaLangString) par_5);
        s = (JavaLangString) UTIL.clone(rhs_4);
        return (JavaLangString) s;
    }

}

