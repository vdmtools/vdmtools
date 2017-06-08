package jp.vdmtools.VDM.jdk;

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;


public abstract class J2VUTIL {

    static public Object Convert(final String p_utype, final Object p_uval)
            throws CGException {
        if (p_utype.equals(new String("JavaLangString")))
            return toJString(p_uval);
        else if (p_utype.equals(new String("char")))
            return toChar(p_uval);
        else if (p_utype.equals(new String("int")))
            return toInt(p_uval);
        else if (p_utype.equals(new String("real")) ||
                p_utype.equals(new String("double")))
            return toFloat(p_uval);
        else {
            UTIL.RunTime("Run-Time Error:Can not evaluate an error statement");
            return new Object();
        }
    }


    static public Integer BitOp(final Integer p_uleft,
                                final Object p_uop,
                                final Integer p_uright)
            throws CGException {
        int lv = p_uleft.intValue();
        int rv = p_uright.intValue();
        int result = 0;
        if (p_uop.equals(new quotes.AND()))
            result = lv & rv;
        else if (p_uop.equals(new quotes.OR()))
            result = lv | rv;
        else if (p_uop.equals(new quotes.EXCLOR()))
            result = lv ^ rv;
        else
            UTIL.RunTime("J2VUTIL.BitOp: Unknown operator");

        return new Integer(result);
    }


    static public JavaLangString ConcatStr(final JavaLangString p_ustr1,
                                           final JavaLangString p_ustr2) {
        // Note here that if a JavaLangString has isNil = true, its
        // wrapped object is the empty string
        StringBuffer l_res = new StringBuffer(p_ustr1.toSeqOfChar());
        l_res.append(p_ustr2.toSeqOfChar());
        return new JavaLangString(l_res.toString());
    }


    static public JavaLangString toJString(final Object p_uval)
            throws CGException {
        Object rexpr_2 = null;
        boolean succ_3 = true;
        {
            succ_3 = true;
            if (!UTIL.equals(new Boolean(true), new Boolean(UTIL.IsInteger(p_uval))))
                succ_3 = false;
            if (succ_3) {
                Integer l_ui = UTIL.NumberToInt(p_uval);
                return new JavaLangInteger(l_ui).vdm_toString();
            }
        }
        if (!succ_3) {
            succ_3 = true;
            if (!UTIL.equals(new Boolean(true), new Boolean((p_uval instanceof Character))))
                succ_3 = false;
            if (succ_3) {
                Character l_uc = (Character) p_uval;
                JavaLangCharacter obj_18 = null;
                obj_18 = (JavaLangCharacter) new JavaLangCharacter(l_uc);
                rexpr_2 = (Object) obj_18.toString();
            }
        }
        if (!succ_3) {
            succ_3 = true;
            Boolean tmpVal_v_20 = null;
            tmpVal_v_20 = new Boolean(p_uval instanceof JavaLangObject);
            if (!UTIL.equals(new Boolean(true), tmpVal_v_20))
                succ_3 = false;
            if (succ_3) {
                JavaLangObject l_uo = (JavaLangObject) p_uval;
                Boolean cond_25 = null;
                cond_25 = l_uo.IsNil();
                if (cond_25.booleanValue())
                    rexpr_2 = (Object) new JavaLangString(new String("null"));
                else
                    rexpr_2 = (Object) l_uo.toString();
            }
        }
        if (!succ_3)
            UTIL.RunTime("Run-Time Error:Can not evaluate an undefined expression");
        return (JavaLangString) rexpr_2;
    }


    static public Character toChar(final Object p_uval) throws CGException {
        Character rexpr_2 = null;
        boolean succ_3 = true;
        {
            succ_3 = true;
            if (!UTIL.equals(new Boolean(true), new Boolean(UTIL.IsInteger(p_uval))))
                succ_3 = false;
            if (succ_3) {
                Integer l_ui = UTIL.NumberToInt(p_uval);
                JavaLangInteger obj_10 = null;
                obj_10 = (JavaLangInteger) new JavaLangInteger(l_ui);
                rexpr_2 = obj_10.charValue();
            }
        }
        if (!succ_3) {
            succ_3 = true;
            if (!UTIL.equals(new Boolean(true), new Boolean((p_uval instanceof Character))))
                succ_3 = false;
            if (succ_3) {
                Character l_uc = (Character) p_uval;
                rexpr_2 = l_uc;
            }
        }
        if (!succ_3) {
            succ_3 = true;
            if (!UTIL.equals(new Boolean(true), new Boolean(UTIL.IsReal(p_uval))))
                succ_3 = false;
            if (succ_3) {
                Double l_uf = UTIL.NumberToReal(p_uval);
                Integer par_20 = null;
                double tmp_22 = ((Number) l_uf).doubleValue();
                par_20 = new Integer(new Double(Math.floor(tmp_22)).intValue());
                rexpr_2 = toChar(par_20);
            }
        }
        if (!succ_3)
            UTIL.RunTime("Run-Time Error:No \'others\' branch in CasesExpr");
        return rexpr_2;
    }


    static public Integer toInt(final Object p_uval) throws CGException {
        Integer rexpr_2 = null;
        boolean succ_3 = true;
        {
            succ_3 = true;
            if (!UTIL.equals(new Boolean(true), new Boolean(UTIL.IsInteger(p_uval))))
                succ_3 = false;
            if (succ_3) {
                Integer l_ui = UTIL.NumberToInt(p_uval);
                rexpr_2 = l_ui;
            }
        }
        if (!succ_3) {
            succ_3 = true;
            if (!UTIL.equals(new Boolean(true), new Boolean((p_uval instanceof Character))))
                succ_3 = false;
            if (succ_3) {
                Character l_uc = (Character) p_uval;
                JavaLangInteger obj_14 = null;
                obj_14 = (JavaLangInteger) new JavaLangInteger(l_uc);
                rexpr_2 = obj_14.intValue();
            }
        }
        if (!succ_3) {
            succ_3 = true;
            if (!UTIL.equals(new Boolean(true), new Boolean(UTIL.IsReal(p_uval))))
                succ_3 = false;
            if (succ_3) {
                Double l_uf = UTIL.NumberToReal(p_uval);
                double tmp_21 = ((Number) l_uf).doubleValue();
                rexpr_2 = new Integer(new Double(Math.floor(tmp_21)).intValue());
            }
        }
        if (!succ_3)
            UTIL.RunTime("Run-Time Error:No \'others\' branch in CasesExpr");
        return rexpr_2;
    }


    static public Double toFloat(final Object p_uval)
            throws CGException {
        Double rexpr_2 = null;
        boolean succ_3 = true;
        {
            succ_3 = true;
            if (!UTIL.equals(new Boolean(true), new Boolean(UTIL.IsInteger(p_uval))))
                succ_3 = false;
            if (succ_3) {
                Integer l_ui = UTIL.NumberToInt(p_uval);
                rexpr_2 = UTIL.NumberToReal(l_ui);
            }
        }
        if (!succ_3) {
            succ_3 = true;
            if (!UTIL.equals(new Boolean(true), new Boolean((p_uval instanceof Character))))
                succ_3 = false;
            if (succ_3) {
                Integer l_uc = UTIL.NumberToInt(p_uval);
                JavaLangInteger obj_14 = null;
                obj_14 = (JavaLangInteger) new JavaLangInteger(l_uc);
                rexpr_2 = UTIL.NumberToReal(obj_14.intValue());
            }
        }
        if (!succ_3) {
            succ_3 = true;
            if (!UTIL.equals(new Boolean(true), new Boolean(UTIL.IsReal(p_uval))))
                succ_3 = false;
            if (succ_3) {
                Double l_uf = UTIL.NumberToReal(p_uval);
                rexpr_2 = l_uf;
            }
        }
        if (!succ_3)
            UTIL.RunTime("Run-Time Error:No \'others\' branch in CasesExpr");
        return rexpr_2;
    }


}

