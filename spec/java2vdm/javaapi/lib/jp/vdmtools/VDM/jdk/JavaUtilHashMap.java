

package jp.vdmtools.VDM.jdk;


import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.UTIL;
import jp.vdmtools.VDM.VDMSpecException;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;


public class JavaUtilHashMap extends JavaLangObject implements JavaUtilMap, JavaLangCloneable, JavaIoSerializable {

    static UTIL.VDMCompare vdmComp = new UTIL.VDMCompare();

    public HashMap table = new HashMap();

    public Integer count = null;

    public Integer threshold = null;

    public Double loadFactor_u = null;

    public Integer modCount = null;

    public static Integer KEYS = Integer.valueOf(0);

    public static Integer VALUES = Integer.valueOf(1);

    public static Integer ENTRIES = Integer.valueOf(2);

    private static EmptyHashIterator emptyHashIterator = null;

    static {
	try {
	    emptyHashIterator = new EmptyHashIterator();
	} catch (CGException e) {
	    System.err.println("WARNING: Unable to initialize JavaUtilHashMap");
	    e.printStackTrace(System.err);
	}
    }

    public Integer size() throws CGException {
        return count;
    }


    public void clear() throws CGException {
        modCount = UTIL.NumberToInt(UTIL.clone(Integer.valueOf(modCount.intValue() + Integer.valueOf(1).intValue())));
        Integer lb_4 = null;
        Integer var1_5 = null;
        HashSet unArg_6 = new HashSet();
        unArg_6.clear();
        unArg_6.addAll(table.keySet());
        var1_5 = Integer.valueOf(unArg_6.size());
        lb_4 = Integer.valueOf(var1_5.intValue() - Integer.valueOf(1).intValue());
        int iby_11 = Integer.valueOf(-Integer.valueOf(1).intValue()).intValue();
        if (iby_11 == 0)
            UTIL.RunTime("Run-Time Error:Step length in indexed for loop was 0");
        for (int ilb_10 = lb_4.intValue(); iby_11 < 0 ? ilb_10 >= 0 : ilb_10 <= 0; ilb_10 += iby_11) {
            Integer i = Integer.valueOf(ilb_10);
            table.put(i, new HMEntry(new quotes.NIL()));
        }
        count = UTIL.NumberToInt(UTIL.clone(Integer.valueOf(0)));
    }


    public JavaUtilSet keySet() throws CGException {
        return new HMKeySet(this);
    }


    public JavaUtilIterator getHashIterator(final Integer type) throws CGException {
        Object rexpr_2 = null;
        if (Boolean.valueOf(count.intValue() == Integer.valueOf(0).intValue()).booleanValue())
            rexpr_2 = emptyHashIterator;
        else
            rexpr_2 = new HashIterator(type, this);
        return (JavaUtilIterator) rexpr_2;
    }


    public JavaUtilHashMap() throws CGException {
        this(Integer.valueOf(11), Double.valueOf(0.75));
    }


    public Boolean isEmpty() throws CGException {
        return Boolean.valueOf(count.intValue() == Integer.valueOf(0).intValue());
    }


    public JavaUtilHashMap(final Integer initialCapacity) throws CGException {
	this (initialCapacity, Double.valueOf(0.75));
    }


    private Integer capacity() throws CGException {
        Integer rexpr_1 = null;
        HashSet unArg_2 = new HashSet();
        unArg_2.clear();
        unArg_2.addAll(table.keySet());
        rexpr_1 = Integer.valueOf(unArg_2.size());
        return rexpr_1;
    }


    public JavaUtilSet entrySet() throws CGException {
        return null;
    }


    public JavaUtilHashMap(final Integer initialCapacity_u, final Double loadFactor_u_1) throws CGException {
	//        this();
        {
            Integer initialCapacity = initialCapacity_u;
            if (Boolean.valueOf((initialCapacity.intValue()) < (Integer.valueOf(0).intValue())).booleanValue()) {
                JavaLangString s = new JavaLangString(new String("Illegal Initial Capacity: "));
                JavaLangString rhs_7 = null;
                JavaLangString par_8 = new JavaLangString(initialCapacity.toString());
                rhs_7 = (JavaLangString) s.concat((JavaLangString) par_8);
                s = (JavaLangString) UTIL.clone(rhs_7);
                throw new VDMSpecException(new JavaLangIllegalArgumentException(s));
            }
            if (Boolean.valueOf((loadFactor_u_1.doubleValue()) <= (Integer.valueOf(0).intValue())).booleanValue()) {
                JavaLangString s = new JavaLangString(new String("Illegal Load factor: "));
                throw new VDMSpecException(new JavaLangIllegalArgumentException(s));
            }
            if (Boolean.valueOf(initialCapacity.intValue() == Integer.valueOf(0).intValue()).booleanValue())
                initialCapacity = UTIL.NumberToInt(UTIL.clone(Integer.valueOf(1)));
            ((JavaUtilHashMap) this).loadFactor_u = UTIL.NumberToReal(UTIL.clone(loadFactor_u_1));
            HashMap rhs_26 = new HashMap();
            HashMap res_m_27 = new HashMap();
            {
                boolean succ_31 = true;
                HashSet e_set_32 = new HashSet();
                e_set_32 = new HashSet();
                int ubi_37 = Integer.valueOf(initialCapacity.intValue() - Integer.valueOf(1).intValue()).intValue();
                for (int count_38 = Integer.valueOf(0).intValue(); count_38 <= ubi_37; count_38++)
                    e_set_32.add(Integer.valueOf(count_38));
                Integer i = null;
                {
                    for (Iterator enum_40 = e_set_32.iterator(); enum_40.hasNext();) {
                        Integer elem_39 = UTIL.NumberToInt(enum_40.next());
                        succ_31 = true;
                        i = elem_39;
                        if (succ_31)
                            res_m_27.put(i, new HMEntry(new quotes.NIL()));
                    }
                }
            }
            rhs_26 = res_m_27;
            table = (HashMap) UTIL.clone(rhs_26);
            Integer rhs_41 = null;
            double tmp_45 = ((Number) Double.valueOf(initialCapacity.intValue() * loadFactor_u_1.doubleValue())).doubleValue();
            rhs_41 = Integer.valueOf(Double.valueOf(Math.floor(tmp_45)).intValue());
            threshold = UTIL.NumberToInt(UTIL.clone(rhs_41));
            count = UTIL.NumberToInt(UTIL.clone(Integer.valueOf(0)));
            modCount = UTIL.NumberToInt(UTIL.clone(Integer.valueOf(0)));
        }
    }


    private Double loadFactor() throws CGException {
        return loadFactor_u;
    }


    public JavaLangObject get(final JavaLangObject key) throws CGException {
        Boolean cond_2 = null;
        Boolean unArg_3 = null;
        unArg_3 = key.IsNil();
        cond_2 = Boolean.valueOf(!unArg_3.booleanValue());
        if (cond_2.booleanValue()) {
            Integer hash = null;
            hash = key.vdm_hashCode();
            Integer index = null;
            Integer var1_15 = null;
            if (Boolean.valueOf((hash.intValue()) >= (Integer.valueOf(0).intValue())).booleanValue())
                var1_15 = hash;
            else
                var1_15 = Integer.valueOf(-hash.intValue());
            Integer var2_21 = null;
            HashSet unArg_22 = new HashSet();
            unArg_22.clear();
            unArg_22.addAll(table.keySet());
            var2_21 = Integer.valueOf(unArg_22.size());
            index = Integer.valueOf((int) (var1_15.doubleValue() - var2_21.doubleValue() * Math.floor(var1_15.doubleValue() / var2_21.doubleValue())));
            HMEntry e = (HMEntry) table.get(index);
            while (true) {
                Boolean whCrtl_26 = null;
                Boolean unArg_27 = null;
                unArg_27 = e.IsNil();
                whCrtl_26 = Boolean.valueOf(!unArg_27.booleanValue());
                if (whCrtl_26.booleanValue()) {
                    Boolean cond_28 = null;
                    Boolean var1_29 = null;
                    Integer var1_30 = null;
                    var1_30 = e.hash;
                    var1_29 = Boolean.valueOf(var1_30.intValue() == hash.intValue());
                    {
                        if ((cond_28 = var1_29).booleanValue()) {
                            Boolean var2_33 = null;
                            JavaLangObject par_34 = null;
                            par_34 = e.key;
                            var2_33 = key.vdm_equals((JavaLangObject) par_34);
                            cond_28 = var2_33;
                        }
                    }
                    if (cond_28.booleanValue()) {
                        JavaLangObject rexpr_36 = null;
                        rexpr_36 = e.value_u_u;
                        return (JavaLangObject) rexpr_36;
                    }
                    HMEntry rhs_38 = null;
                    rhs_38 = e.next;
                    e = (HMEntry) UTIL.clone(rhs_38);
                } else
                    break;
            }
        } else {
            HMEntry e = (HMEntry) table.get(Integer.valueOf(0));
            while (true) {
                Boolean whCrtl_6 = null;
                Boolean unArg_7 = null;
                unArg_7 = e.IsNil();
                whCrtl_6 = Boolean.valueOf(!unArg_7.booleanValue());
                if (whCrtl_6.booleanValue()) {
                    Boolean cond_8 = null;
                    JavaLangObject obj_9 = null;
                    obj_9 = e.key;
                    cond_8 = obj_9.IsNil();
                    if (cond_8.booleanValue()) {
                        JavaLangObject rexpr_11 = null;
                        rexpr_11 = e.value_u_u;
                        return (JavaLangObject) rexpr_11;
                    }
                    HMEntry rhs_13 = null;
                    rhs_13 = e.next;
                    e = (HMEntry) UTIL.clone(rhs_13);
                } else
                    break;
            }
        }
        return (JavaLangObject) new JavaLangObject(new quotes.NIL());
    }


    public JavaUtilHashMap(final JavaUtilMap t) throws CGException {
        this();
        ;
    }


    public JavaUtilHashMap(final quotes.NIL var_1_1) throws CGException {
        this();
        isNil = Boolean.TRUE;
    }


    public JavaLangObject remove(final JavaLangObject key) throws CGException {
        Boolean cond_2 = null;
        Boolean unArg_3 = null;
        unArg_3 = key.IsNil();
        cond_2 = Boolean.valueOf(!unArg_3.booleanValue());
        if (cond_2.booleanValue()) {
            Integer hash = null;
            hash = key.vdm_hashCode();
            Integer index = null;
            Integer var1_35 = null;
            if (Boolean.valueOf((hash.intValue()) < (Integer.valueOf(0).intValue())).booleanValue())
                var1_35 = Integer.valueOf(-hash.intValue());
            else
                var1_35 = hash;
            Integer var2_41 = null;
            HashSet unArg_42 = new HashSet();
            unArg_42.clear();
            unArg_42.addAll(table.keySet());
            var2_41 = Integer.valueOf(unArg_42.size());
            index = Integer.valueOf((int) (var1_35.doubleValue() - var2_41.doubleValue() * Math.floor(var1_35.doubleValue() / var2_41.doubleValue())));
            {
                HMEntry e = (HMEntry) table.get(index);
                HMEntry prev = new HMEntry(new quotes.NIL());
                while (true) {
                    Boolean whCrtl_47 = null;
                    Boolean unArg_48 = null;
                    unArg_48 = e.IsNil();
                    whCrtl_47 = Boolean.valueOf(!unArg_48.booleanValue());
                    if (whCrtl_47.booleanValue()) {
                        Boolean cond_49 = null;
                        Boolean var1_50 = null;
                        Integer var1_51 = null;
                        var1_51 = e.hash;
                        var1_50 = Boolean.valueOf(var1_51.intValue() == hash.intValue());
                        {
                            if ((cond_49 = var1_50).booleanValue()) {
                                Boolean var2_54 = null;
                                JavaLangObject obj_55 = null;
                                obj_55 = e.key;
                                var2_54 = obj_55.vdm_equals((JavaLangObject) key);
                                cond_49 = var2_54;
                            }
                        }
                        if (cond_49.booleanValue()) {
                            modCount = UTIL.NumberToInt(UTIL.clone(Integer.valueOf(modCount.intValue() + Integer.valueOf(1).intValue())));
                            Boolean cond_61 = null;
                            Boolean unArg_62 = null;
                            unArg_62 = prev.IsNil();
                            cond_61 = Boolean.valueOf(!unArg_62.booleanValue());
                            if (cond_61.booleanValue()) {
                                HMEntry rhs_67 = null;
                                rhs_67 = e.next;
                                prev.next = (HMEntry) rhs_67;
                            } else {
                                HMEntry mr_65 = null;
                                mr_65 = e.next;
                                table.put(index, mr_65);
                            }
                            count = UTIL.NumberToInt(UTIL.clone(Integer.valueOf(count.intValue() - Integer.valueOf(1).intValue())));
                            {
                                JavaLangObject oldValue = null;
                                oldValue = e.value_u_u;
                                e.value_u_u = (JavaLangObject) new JavaLangObject(new quotes.NIL());
                                return (JavaLangObject) oldValue;
                            }
                        }
                        prev = (HMEntry) UTIL.clone(e);
                        HMEntry rhs_79 = null;
                        rhs_79 = e.next;
                        e = (HMEntry) UTIL.clone(rhs_79);
                    } else
                        break;
                }
            }
        } else {
            HMEntry e = (HMEntry) table.get(Integer.valueOf(0));
            HMEntry prev = new HMEntry(new quotes.NIL());
            while (true) {
                Boolean whCrtl_7 = null;
                Boolean unArg_8 = null;
                unArg_8 = e.IsNil();
                whCrtl_7 = Boolean.valueOf(!unArg_8.booleanValue());
                if (whCrtl_7.booleanValue()) {
                    Boolean cond_9 = null;
                    JavaLangObject obj_10 = null;
                    obj_10 = e.key;
                    cond_9 = obj_10.IsNil();
                    if (cond_9.booleanValue()) {
                        modCount = UTIL.NumberToInt(UTIL.clone(Integer.valueOf(modCount.intValue() + Integer.valueOf(1).intValue())));
                        Boolean cond_15 = null;
                        Boolean unArg_16 = null;
                        unArg_16 = prev.IsNil();
                        cond_15 = Boolean.valueOf(!unArg_16.booleanValue());
                        if (cond_15.booleanValue()) {
                            HMEntry rhs_21 = null;
                            rhs_21 = e.next;
                            prev.next = (HMEntry) rhs_21;
                        } else {
                            HMEntry mr_19 = null;
                            mr_19 = e.next;
                            table.put(Integer.valueOf(0), mr_19);
                        }
                        count = UTIL.NumberToInt(UTIL.clone(Integer.valueOf(count.intValue() - Integer.valueOf(1).intValue())));
                        {
                            JavaLangObject oldValue = null;
                            oldValue = e.value_u_u;
                            e.value_u_u = (JavaLangObject) new JavaLangObject(new quotes.NIL());
                            return (JavaLangObject) oldValue;
                        }
                    }
                    prev = (HMEntry) UTIL.clone(e);
                    HMEntry rhs_33 = null;
                    rhs_33 = e.next;
                    e = (HMEntry) UTIL.clone(rhs_33);
                } else
                    break;
            }
        }
        return (JavaLangObject) new JavaLangObject(new quotes.NIL());
    }


    public Boolean containsKey(final JavaLangObject key) throws CGException {
        Boolean cond_2 = null;
        Boolean unArg_3 = null;
        unArg_3 = key.IsNil();
        cond_2 = Boolean.valueOf(!unArg_3.booleanValue());
        if (cond_2.booleanValue()) {
            Integer hash = null;
            hash = key.vdm_hashCode();
            Integer index = null;
            Integer var1_14 = null;
            if (Boolean.valueOf((hash.intValue()) < (Integer.valueOf(0).intValue())).booleanValue())
                var1_14 = Integer.valueOf(-hash.intValue());
            else
                var1_14 = hash;
            Integer var2_20 = null;
            HashSet unArg_21 = new HashSet();
            unArg_21.clear();
            unArg_21.addAll(table.keySet());
            var2_20 = Integer.valueOf(unArg_21.size());
            index = Integer.valueOf((int) (var1_14.doubleValue() - var2_20.doubleValue() * Math.floor(var1_14.doubleValue() / var2_20.doubleValue())));
            {
                HMEntry e = (HMEntry) table.get(index);
                while (true) {
                    Boolean whCrtl_25 = null;
                    Boolean unArg_26 = null;
                    unArg_26 = e.IsNil();
                    whCrtl_25 = Boolean.valueOf(!unArg_26.booleanValue());
                    if (whCrtl_25.booleanValue()) {
                        Boolean cond_27 = null;
                        Boolean var1_28 = null;
                        Integer var1_29 = null;
                        var1_29 = e.hash;
                        var1_28 = Boolean.valueOf(var1_29.intValue() == hash.intValue());
                        {
                            if ((cond_27 = var1_28).booleanValue()) {
                                Boolean var2_32 = null;
                                JavaLangObject obj_33 = null;
                                obj_33 = e.key;
                                var2_32 = obj_33.vdm_equals((JavaLangObject) key);
                                cond_27 = var2_32;
                            }
                        }
                        if (cond_27.booleanValue())
                            return Boolean.TRUE;
                        HMEntry rhs_37 = null;
                        rhs_37 = e.next;
                        e = (HMEntry) UTIL.clone(rhs_37);
                    } else
                        break;
                }
            }
        } else {
            HMEntry e = (HMEntry) table.get(Integer.valueOf(0));
            while (true) {
                Boolean whCrtl_6 = null;
                Boolean unArg_7 = null;
                unArg_7 = e.IsNil();
                whCrtl_6 = Boolean.valueOf(!unArg_7.booleanValue());
                if (whCrtl_6.booleanValue()) {
                    Boolean cond_8 = null;
                    JavaLangObject obj_9 = null;
                    obj_9 = e.key;
                    cond_8 = obj_9.IsNil();
                    if (cond_8.booleanValue())
                        return Boolean.TRUE;
                    HMEntry rhs_12 = null;
                    rhs_12 = e.next;
                    e = (HMEntry) UTIL.clone(rhs_12);
                } else
                    break;
            }
        }
        return Boolean.FALSE;
    }


    public JavaLangObject put(final JavaLangObject key, final JavaLangObject value_u_u) throws CGException {
        Integer hash = Integer.valueOf(0);
        Integer index = Integer.valueOf(0);
        Boolean cond_3 = null;
        Boolean unArg_4 = null;
        unArg_4 = key.IsNil();
        cond_3 = Boolean.valueOf(!unArg_4.booleanValue());
        if (cond_3.booleanValue()) {
            Integer rhs_18 = null;
            rhs_18 = key.vdm_hashCode();
            hash = UTIL.NumberToInt(UTIL.clone(rhs_18));
            Integer rhs_19 = null;
            Integer var1_20 = null;
            if (Boolean.valueOf((hash.intValue()) < (Integer.valueOf(0).intValue())).booleanValue())
                var1_20 = Integer.valueOf(-hash.intValue());
            else
                var1_20 = hash;
            Integer var2_26 = null;
            HashSet unArg_27 = new HashSet();
            unArg_27.clear();
            unArg_27.addAll(table.keySet());
            var2_26 = Integer.valueOf(unArg_27.size());
            rhs_19 = Integer.valueOf((int) (var1_20.doubleValue() - var2_26.doubleValue() * Math.floor(var1_20.doubleValue() / var2_26.doubleValue())));
            index = UTIL.NumberToInt(UTIL.clone(rhs_19));
            {
                HMEntry e = (HMEntry) table.get(index);
                while (true) {
                    Boolean whCrtl_31 = null;
                    Boolean unArg_32 = null;
                    unArg_32 = e.IsNil();
                    whCrtl_31 = Boolean.valueOf(!unArg_32.booleanValue());
                    if (whCrtl_31.booleanValue()) {
                        JavaLangObject old = null;
                        old = e.value_u_u;
                        Boolean cond_34 = null;
                        Boolean var1_35 = null;
                        Integer var1_36 = null;
                        var1_36 = e.hash;
                        var1_35 = Boolean.valueOf(var1_36.intValue() == hash.intValue());
                        {
                            if ((cond_34 = var1_35).booleanValue()) {
                                Boolean var2_39 = null;
                                JavaLangObject obj_40 = null;
                                obj_40 = e.key;
                                var2_39 = obj_40.vdm_equals((JavaLangObject) key);
                                cond_34 = var2_39;
                            }
                        }
                        if (cond_34.booleanValue()) {
                            e.value_u_u = (JavaLangObject) value_u_u;
                            return (JavaLangObject) old;
                        }
                        HMEntry rhs_46 = null;
                        rhs_46 = e.next;
                        e = (HMEntry) UTIL.clone(rhs_46);
                    } else
                        break;
                }
            }
        } else {
            HMEntry e = (HMEntry) table.get(Integer.valueOf(0));
            while (true) {
                Boolean whCrtl_7 = null;
                Boolean unArg_8 = null;
                unArg_8 = e.IsNil();
                whCrtl_7 = Boolean.valueOf(!unArg_8.booleanValue());
                if (whCrtl_7.booleanValue()) {
                    Boolean cond_9 = null;
                    JavaLangObject obj_10 = null;
                    obj_10 = e.key;
                    cond_9 = obj_10.IsNil();
                    if (cond_9.booleanValue()) {
                        JavaLangObject old = null;
                        old = e.value_u_u;
                        e.value_u_u = (JavaLangObject) value_u_u;
                        return (JavaLangObject) old;
                    }
                    HMEntry rhs_16 = null;
                    rhs_16 = e.next;
                    e = (HMEntry) UTIL.clone(rhs_16);
                } else
                    break;
            }
        }
        modCount = UTIL.NumberToInt(UTIL.clone(Integer.valueOf(modCount.intValue() + Integer.valueOf(1).intValue())));
        if (Boolean.valueOf((count.intValue()) >= (threshold.intValue())).booleanValue()) {
            rehash();
            Integer rhs_55 = null;
            Integer var1_56 = null;
            if (Boolean.valueOf((hash.intValue()) < (Integer.valueOf(0).intValue())).booleanValue())
                var1_56 = Integer.valueOf(-hash.intValue());
            else
                var1_56 = hash;
            Integer var2_62 = null;
            HashSet unArg_63 = new HashSet();
            unArg_63.clear();
            unArg_63.addAll(table.keySet());
            var2_62 = Integer.valueOf(unArg_63.size());
            rhs_55 = Integer.valueOf((int) (var1_56.doubleValue() - var2_62.doubleValue() * Math.floor(var1_56.doubleValue() / var2_62.doubleValue())));
            index = UTIL.NumberToInt(UTIL.clone(rhs_55));
        }
        {
            HMEntry e = new HMEntry(hash, key, value_u_u, (HMEntry) table.get(index));
            table.put(index, e);
            count = UTIL.NumberToInt(UTIL.clone(Integer.valueOf(count.intValue() + Integer.valueOf(1).intValue())));
            return (JavaLangObject) new JavaLangObject(new quotes.NIL());
        }
    }


    protected void rehash() throws CGException {
        Integer oldCapacity = null;
        HashSet unArg_1 = new HashSet();
        unArg_1.clear();
        unArg_1.addAll(table.keySet());
        oldCapacity = Integer.valueOf(unArg_1.size());
        Integer newCapacity = Integer.valueOf(Integer.valueOf(oldCapacity.intValue() * Integer.valueOf(2).intValue()).intValue() + Integer.valueOf(1).intValue());
        HashMap newMap = new HashMap();
        HashMap res_m_7 = new HashMap();
        {
            boolean succ_11 = true;
            HashSet e_set_12 = new HashSet();
            e_set_12 = new HashSet();
            int ubi_17 = Integer.valueOf(newCapacity.intValue() - Integer.valueOf(1).intValue()).intValue();
            for (int count_18 = Integer.valueOf(0).intValue(); count_18 <= ubi_17; count_18++)
                e_set_12.add(Integer.valueOf(count_18));
            Integer l_u10 = null;
            {
                for (Iterator enum_20 = e_set_12.iterator(); enum_20.hasNext();) {
                    Integer elem_19 = UTIL.NumberToInt(enum_20.next());
                    succ_11 = true;
                    l_u10 = elem_19;
                    if (succ_11)
                        res_m_7.put(l_u10, new HMEntry(new quotes.NIL()));
                }
            }
        }
        newMap = res_m_7;
        modCount = UTIL.NumberToInt(UTIL.clone(Integer.valueOf(modCount.intValue() + Integer.valueOf(1).intValue())));
        {
            Double l_u6 = Double.valueOf(newCapacity.intValue() * loadFactor_u.doubleValue());
            Double l_u1 = null;
            Boolean cond_30 = null;
	    double tmp_32 = ((Number) l_u6).doubleValue();
	    l_u1 = Double.valueOf(Double.valueOf(Math.floor(tmp_32)).intValue());
            threshold = UTIL.NumberToInt(UTIL.clone(l_u1));
        }
        {
            Integer i = oldCapacity;
            Boolean l_u11 = null;
            {
                Integer l_u3 = i;
                Integer l_u2 = l_u3;
                {
                    i = UTIL.NumberToInt(UTIL.clone(Integer.valueOf(l_u3.intValue() - Integer.valueOf(1).intValue())));
                    {
                        Boolean l_u1 = Boolean.valueOf((l_u2.intValue()) > (Integer.valueOf(0).intValue()));
                        l_u11 = (Boolean) UTIL.clone(l_u1);
                    }
                }
            }
            while (l_u11.booleanValue()) {
                {
                    HMEntry old = (HMEntry) table.get(i);
                    while (true) {
                        Boolean whCrtl_48 = null;
                        Boolean unArg_49 = null;
                        unArg_49 = old.IsNil();
                        whCrtl_48 = Boolean.valueOf(!unArg_49.booleanValue());
                        if (whCrtl_48.booleanValue()) {
                            HMEntry e = old;
                            HMEntry rhs_50 = null;
                            rhs_50 = old.next;
                            old = (HMEntry) UTIL.clone(rhs_50);
                            {
                                Integer index = null;
                                Integer var1_52 = null;
                                {
                                    Integer i_1 = null;
                                    i_1 = e.hash;
                                    if (Boolean.valueOf((i_1.intValue()) < (Integer.valueOf(0).intValue())).booleanValue())
                                        var1_52 = Integer.valueOf(-i_1.intValue());
                                    else
                                        var1_52 = i_1;
                                }
                                index = Integer.valueOf((int) (var1_52.doubleValue() - newCapacity.doubleValue() * Math.floor(var1_52.doubleValue() / newCapacity.doubleValue())));
                                e.next = (HMEntry) (HMEntry) newMap.get(index);
                                newMap.put(index, e);
                            }
                        } else
                            break;
                    }
                }
                {
                    Integer l_u3 = i;
                    Integer l_u2 = l_u3;
                    {
                        i = UTIL.NumberToInt(UTIL.clone(Integer.valueOf(l_u3.intValue() - Integer.valueOf(1).intValue())));
                        {
                            Boolean l_u1 = Boolean.valueOf((l_u2.intValue()) > (Integer.valueOf(0).intValue()));
                            l_u11 = (Boolean) UTIL.clone(l_u1);
                        }
                    }
                }
            }
        }
        table = (HashMap) UTIL.clone(newMap);
        ;
    }


    public Boolean containsValue(final JavaLangObject value_u_u) throws CGException {
        Boolean cond_2 = null;
        cond_2 = value_u_u.IsNil();
        if (cond_2.booleanValue()) {
            Integer lb_23 = null;
            Integer var1_24 = null;
            HashSet unArg_25 = new HashSet();
            unArg_25.clear();
            unArg_25.addAll(table.keySet());
            var1_24 = Integer.valueOf(unArg_25.size());
            lb_23 = Integer.valueOf(var1_24.intValue() - Integer.valueOf(1).intValue());
            int iby_30 = Integer.valueOf(-Integer.valueOf(1).intValue()).intValue();
            if (iby_30 == 0)
                UTIL.RunTime("Run-Time Error:Step length in indexed for loop was 0");
            for (int ilb_29 = lb_23.intValue(); iby_30 < 0 ? ilb_29 >= 0 : ilb_29 <= 0; ilb_29 += iby_30) {
                Integer i = Integer.valueOf(ilb_29);
                {
                    HMEntry e = (HMEntry) table.get(i);
                    while (true) {
                        Boolean whCrtl_35 = null;
                        Boolean unArg_36 = null;
                        unArg_36 = e.IsNil();
                        whCrtl_35 = Boolean.valueOf(!unArg_36.booleanValue());
                        if (whCrtl_35.booleanValue()) {
                            Boolean cond_37 = null;
                            JavaLangObject obj_38 = null;
                            obj_38 = e.value_u_u;
                            cond_37 = obj_38.IsNil();
                            if (cond_37.booleanValue())
                                return Boolean.TRUE;
                            HMEntry rhs_41 = null;
                            rhs_41 = e.next;
                            e = (HMEntry) UTIL.clone(rhs_41);
                        } else
                            break;
                    }
                }
            }
        } else {
            Integer lb_3 = null;
            Integer var1_4 = null;
            HashSet unArg_5 = new HashSet();
            unArg_5.clear();
            unArg_5.addAll(table.keySet());
            var1_4 = Integer.valueOf(unArg_5.size());
            lb_3 = Integer.valueOf(var1_4.intValue() - Integer.valueOf(1).intValue());
            int iby_10 = Integer.valueOf(-Integer.valueOf(1).intValue()).intValue();
            if (iby_10 == 0)
                UTIL.RunTime("Run-Time Error:Step length in indexed for loop was 0");
            for (int ilb_9 = lb_3.intValue(); iby_10 < 0 ? ilb_9 >= 0 : ilb_9 <= 0; ilb_9 += iby_10) {
                Integer i = Integer.valueOf(ilb_9);
                {
                    HMEntry e = (HMEntry) table.get(i);
                    while (true) {
                        Boolean whCrtl_15 = null;
                        Boolean unArg_16 = null;
                        unArg_16 = e.IsNil();
                        whCrtl_15 = Boolean.valueOf(!unArg_16.booleanValue());
                        if (whCrtl_15.booleanValue()) {
                            Boolean cond_17 = null;
                            JavaLangObject par_18 = null;
                            par_18 = e.value_u_u;
                            cond_17 = value_u_u.vdm_equals((JavaLangObject) par_18);
                            if (cond_17.booleanValue())
                                return Boolean.TRUE;
                            HMEntry rhs_21 = null;
                            rhs_21 = e.next;
                            e = (HMEntry) UTIL.clone(rhs_21);
                        } else
                            break;
                    }
                }
            }
        }
        return Boolean.FALSE;
    }

}

