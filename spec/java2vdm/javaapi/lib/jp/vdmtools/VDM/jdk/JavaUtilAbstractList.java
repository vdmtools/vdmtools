package jp.vdmtools.VDM.jdk;

// ***** VDMTOOLS END Name=package


// ***** VDMTOOLS START Name=imports KEEP=NO

import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;
import jp.vdmtools.VDM.VDMSpecException;

// ***** VDMTOOLS END Name=imports


public abstract class JavaUtilAbstractList extends JavaLangObject implements JavaUtilList {

// ***** VDMTOOLS START Name=vdmComp KEEP=NO
    static UTIL.VDMCompare vdmComp = new UTIL.VDMCompare();
// ***** VDMTOOLS END Name=vdmComp

// ***** VDMTOOLS START Name=modCount KEEP=NO
    public Integer modCount = null;
// ***** VDMTOOLS END Name=modCount

// ***** VDMTOOLS START Name=CLASS KEEP=NO
    private static final JavaLangClass CLASS = (JavaLangClass) new JavaLangClass(new String("JavaUtilAbstractList"), new quotes.CLASS(), new Boolean(false));
// ***** VDMTOOLS END Name=CLASS


// ***** VDMTOOLS START Name=JavaUtilAbstractList KEEP=NO
    public JavaUtilAbstractList() throws CGException {

            modCount = new Integer(0);

    }
// ***** VDMTOOLS END Name=JavaUtilAbstractList


// ***** VDMTOOLS START Name=get KEEP=NO
    abstract public JavaLangObject get(final Integer index) throws CGException;
// ***** VDMTOOLS END Name=get


// ***** VDMTOOLS START Name=clear KEEP=NO
    public void clear() throws CGException {
        removeRange(new Integer(0), size());
    }
// ***** VDMTOOLS END Name=clear


// ***** VDMTOOLS START Name=getClass KEEP=NO
    public JavaLangClass getJClass() throws CGException {
        return (JavaLangClass) CLASS;
    }
// ***** VDMTOOLS END Name=getClass


// ***** VDMTOOLS START Name=hashCode KEEP=NO
    public Integer jHashCode() throws CGException {
        Integer hashCode_1 = new Integer(1);
        JavaUtilIterator i = iterator();
        while (true) {
            Boolean whCrtl_1 = null;
            whCrtl_1 = i.hasNext();
            if (whCrtl_1.booleanValue()) {
                JavaLangObject obj = null;
                obj = (JavaLangObject) i.next();
                ;
            } else
                break;
        }
        return hashCode_1;
    }
// ***** VDMTOOLS END Name=hashCode


// ***** VDMTOOLS START Name=iterator KEEP=NO
    public JavaUtilIterator iterator() throws CGException {
        return null;
    }
// ***** VDMTOOLS END Name=iterator


// ***** VDMTOOLS START Name=add KEEP=NO
    abstract public Boolean add(final JavaLangObject o) throws CGException;
// ***** VDMTOOLS END Name=add


// ***** VDMTOOLS START Name=listIterator KEEP=NO
    public JavaUtilListIterator listIterator() throws CGException {
        JavaUtilListIterator rexpr_1 = null;
        rexpr_1 = (JavaUtilListIterator) listIterator(new Integer(0));
        return (JavaUtilListIterator) rexpr_1;
    }
// ***** VDMTOOLS END Name=listIterator


// ***** VDMTOOLS START Name=listIterator KEEP=NO
    public JavaUtilListIterator listIterator(final Integer index) throws CGException {
        JavaLangString s = new JavaLangString(new String("Index: "));
        JavaLangString rhs_3 = null;
        JavaLangString par_4 = new JavaLangInteger(index).vdm_toString();
        rhs_3 = (JavaLangString) s.concat((JavaLangString) par_4);
        s = (JavaLangString) UTIL.clone(rhs_3);
        Boolean cond_9 = null;
        {
            if (!(cond_9 = new Boolean((index.intValue()) < (new Integer(0).intValue()))).booleanValue())
                cond_9 = new Boolean((index.intValue()) > (size().intValue()));
        }
        if (cond_9.booleanValue())
            throw new VDMSpecException(new JavaLangIndexOutOfBoundsException(s));
        return null;
    }
// ***** VDMTOOLS END Name=listIterator


// ***** VDMTOOLS START Name=equals KEEP=NO
    public Boolean equals(final JavaLangObject o) throws CGException {
        if (new Boolean(!new Boolean(o instanceof JavaUtilAbstractList).booleanValue()).booleanValue())
            return new Boolean(false);
        {
            JavaUtilListIterator e1 = listIterator();
            JavaUtilListIterator e2 = ((JavaUtilAbstractList) o).listIterator();
            while (true) {
                Boolean whCrtl_11 = null;
                Boolean var1_12 = null;
                var1_12 = e1.hasNext();
                {
                    if ((whCrtl_11 = var1_12).booleanValue()) {
                        Boolean var2_13 = null;
                        var2_13 = e2.hasNext();
                        whCrtl_11 = var2_13;
                    }
                }
                if (whCrtl_11.booleanValue()) {
                    JavaLangObject o1 = null;
                    o1 = (JavaLangObject) e1.next();
                    JavaLangObject o2 = null;
                    o2 = (JavaLangObject) e2.next();
                    return new Boolean(false);
                } else
                    break;
            }
            Boolean rexpr_15 = null;
            Boolean unArg_16 = null;
            Boolean var1_17 = null;
            var1_17 = e1.hasNext();
            {
                if (!(unArg_16 = var1_17).booleanValue()) {
                    Boolean var2_18 = null;
                    var2_18 = e2.hasNext();
                    unArg_16 = var2_18;
                }
            }
            rexpr_15 = new Boolean(!unArg_16.booleanValue());
            return rexpr_15;
        }
    }
// ***** VDMTOOLS END Name=equals


// ***** VDMTOOLS START Name=removeRange KEEP=NO
    protected void removeRange(final Integer fromIndex, final Integer toIndex) throws CGException {
        JavaUtilListIterator it = (JavaUtilListIterator) listIterator(fromIndex);
        {
            Integer i = new Integer(0);
            Integer n = new Integer(toIndex.intValue() - fromIndex.intValue());
            while (new Boolean((i.intValue()) < (n.intValue())).booleanValue()) {
                {
                    JavaLangObject tmpVal_10 = null;
                    tmpVal_10 = (JavaLangObject) it.next();
                    ;
                }
                it.remove();
                i = UTIL.NumberToInt(UTIL.clone(new Integer(i.intValue() + new Integer(1).intValue())));
            }
        }
        ;
    }
// ***** VDMTOOLS END Name=removeRange


// ***** VDMTOOLS START Name=indexOf KEEP=NO
    public Integer indexOf(final JavaLangObject o) throws CGException {
        JavaUtilListIterator e = listIterator();
        Boolean cond_2 = null;
        cond_2 = o.IsNil();
        if (cond_2.booleanValue())
            while (true) {
                Boolean whCrtl_7 = null;
                whCrtl_7 = e.hasNext();
                if (whCrtl_7.booleanValue()) {
                    Boolean cond_8 = null;
                    JavaLangObject obj_9 = null;
                    obj_9 = (JavaLangObject) e.next();
                    cond_8 = obj_9.IsNil();
                    if (cond_8.booleanValue()) {
                        Integer rexpr_10 = null;
                        rexpr_10 = e.previousIndex();
                        return rexpr_10;
                    }
                } else
                    break;
            }
        else
            while (true) {
                Boolean whCrtl_3 = null;
                whCrtl_3 = e.hasNext();
                if (whCrtl_3.booleanValue()) {
                    Boolean cond_4 = null;
                    JavaLangObject par_5 = null;
                    par_5 = (JavaLangObject) e.next();
                    cond_4 = o.vdm_equals((JavaLangObject) par_5);
                    if (cond_4.booleanValue()) {
                        Integer rexpr_6 = null;
                        rexpr_6 = e.previousIndex();
                        return rexpr_6;
                    }
                } else
                    break;
            }
        return new Integer(-new Integer(1).intValue());
    }
// ***** VDMTOOLS END Name=indexOf


// ***** VDMTOOLS START Name=set_u_u KEEP=NO
    public JavaLangObject set_u_u(final Integer index, final JavaLangObject element) throws CGException {
        throw new VDMSpecException(new JavaLangUnsupportedOperationException());
    }
// ***** VDMTOOLS END Name=set_u_u


// ***** VDMTOOLS START Name=JavaUtilAbstractList KEEP=NO
    public JavaUtilAbstractList(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = (Boolean) UTIL.clone(new Boolean(true));
    }
// ***** VDMTOOLS END Name=JavaUtilAbstractList


// ***** VDMTOOLS START Name=lastIndexOf KEEP=NO
    public Integer lastIndexOf(final JavaLangObject o) throws CGException {
        JavaUtilListIterator e = (JavaUtilListIterator) listIterator(size());
        Boolean cond_3 = null;
        cond_3 = o.IsNil();
        if (cond_3.booleanValue())
            while (true) {
                Boolean whCrtl_8 = null;
                whCrtl_8 = e.hasPrevious();
                if (whCrtl_8.booleanValue()) {
                    Boolean cond_9 = null;
                    JavaLangObject obj_10 = null;
                    obj_10 = (JavaLangObject) e.previous();
                    cond_9 = obj_10.IsNil();
                    if (cond_9.booleanValue()) {
                        Integer rexpr_11 = null;
                        rexpr_11 = e.nextIndex();
                        return rexpr_11;
                    }
                } else
                    break;
            }
        else
            while (true) {
                Boolean whCrtl_4 = null;
                whCrtl_4 = e.hasPrevious();
                if (whCrtl_4.booleanValue()) {
                    Boolean cond_5 = null;
                    JavaLangObject par_6 = null;
                    par_6 = (JavaLangObject) e.previous();
                    cond_5 = o.vdm_equals((JavaLangObject) par_6);
                    if (cond_5.booleanValue()) {
                        Integer rexpr_7 = null;
                        rexpr_7 = e.nextIndex();
                        return rexpr_7;
                    }
                } else
                    break;
            }
        return new Integer(-new Integer(1).intValue());
    }
// ***** VDMTOOLS END Name=lastIndexOf

}

;
