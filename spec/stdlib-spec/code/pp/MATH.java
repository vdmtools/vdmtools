// ***** VDMTOOLS START Name=HeaderComment KEEP=NO
// Implementation of the VDM standard MATH library
// Use this file instead of the automatically generated MATH.java file
// $Revision: 1.2 $
// $Date: 2005/05/11 06:45:49 $

// Note that there is no concurrency code, since all of the methods
// correspond to VDM++ functions, which do not have associated history
// counters.

// ***** VDMTOOLS END Name=HeaderComment

// ***** VDMTOOLS START Name=package KEEP=YES
// ***** VDMTOOLS END Name=package


// ***** VDMTOOLS START Name=imports KEEP=YES
import jp.vdmtools.VDM.UTIL;
import jp.vdmtools.VDM.CGException;
import jp.vdmtools.VDM.RANDOM;
// ***** VDMTOOLS END Name=imports

public class MATH {

// ***** VDMTOOLS START Name=pi KEEP=YES
  public static final Double pi = new Double(Math.PI);
  public static final RANDOM rd = new RANDOM();
  public static long rnd_seed = -1;
// ***** VDMTOOLS END Name=pi


// ***** VDMTOOLS START Name=sin#1|Number KEEP=YES
  public static Double sin (final Number v) throws CGException {
    return Double.valueOf(Math.sin(v.doubleValue()));
  }
// ***** VDMTOOLS END Name=sin#1|Number


// ***** VDMTOOLS START Name=post_sin#1|Number KEEP=YES
  public static Boolean post_sin (final Number v, final Double RESULT) throws CGException {
    return Boolean.valueOf(Math.abs(RESULT.doubleValue()) <= 1);
  }
// ***** VDMTOOLS END Name=post_sin#1|Number


// ***** VDMTOOLS START Name=cos#1|Number KEEP=YES
  public static Double cos (final Number v) throws CGException {
    return Double.valueOf(Math.cos(v.doubleValue()));
  }
// ***** VDMTOOLS END Name=cos#1|Number


// ***** VDMTOOLS START Name=post_cos#1|Number KEEP=YES
  public static Boolean post_cos (final Number v, final Double RESULT) throws CGException {
    return Boolean.valueOf(Math.abs(RESULT.doubleValue()) <= 1);
  }
// ***** VDMTOOLS END Name=post_cos#1|Number


// ***** VDMTOOLS START Name=tan#1|Number KEEP=YES
  public static Double tan (final Number a) throws CGException {
    return Double.valueOf(Math.tan(UTIL.NumberToReal(a).doubleValue()));
  }
// ***** VDMTOOLS END Name=tan#1|Number


// ***** VDMTOOLS START Name=pre_tan#1|Number KEEP=YES
  public static Boolean pre_tan (final Number a) throws CGException {
    return Boolean.valueOf(cos(a).doubleValue() != 0);
  }
// ***** VDMTOOLS END Name=pre_tan#1|Number


// ***** VDMTOOLS START Name=cot#1|Number KEEP=YES
  public static Double cot (final Number a) throws CGException {
    double ad = UTIL.NumberToReal(a).doubleValue();
    return Double.valueOf(Math.cos(ad)/Math.sin(ad));
  }
// ***** VDMTOOLS END Name=cot#1|Number


// ***** VDMTOOLS START Name=pre_cot#1|Number KEEP=YES
  public static Boolean pre_cot (final Number a) throws CGException {
    return Boolean.valueOf(sin(a).doubleValue() != 0);
  }
// ***** VDMTOOLS END Name=pre_cot#1|Number


// ***** VDMTOOLS START Name=asin#1|Number KEEP=YES
  public static Double asin (final Number a) throws CGException {
    return Double.valueOf(Math.asin(UTIL.NumberToReal(a).doubleValue()));
  }
// ***** VDMTOOLS END Name=asin#1|Number


// ***** VDMTOOLS START Name=pre_asin#1|Number KEEP=YES
  public static Boolean pre_asin (final Number a) throws CGException {
    return Boolean.valueOf(Math.abs(UTIL.NumberToReal(a).doubleValue()) <= 1);
  }
// ***** VDMTOOLS END Name=pre_asin#1|Number


// ***** VDMTOOLS START Name=acos#1|Number KEEP=YES
  public static Double acos (final Number a) throws CGException {
    return Double.valueOf(Math.acos(UTIL.NumberToReal(a).doubleValue()));
  }
// ***** VDMTOOLS END Name=acos#1|Number


// ***** VDMTOOLS START Name=pre_acos#1|Number KEEP=YES
  public static Boolean pre_acos (final Number a) throws CGException {
    return Boolean.valueOf(Math.abs(UTIL.NumberToReal(a).doubleValue()) <= 1);
  }
// ***** VDMTOOLS END Name=pre_acos#1|Number


// ***** VDMTOOLS START Name=atan#1|Number KEEP=YES
  public static Double atan (final Number v) throws CGException {
    return Double.valueOf(Math.atan(v.doubleValue()));
  }
// ***** VDMTOOLS END Name=atan#1|Number


// ***** VDMTOOLS START Name=acot#1|Number KEEP=YES
  public static Double acot (final Number a) throws CGException {
    return atan(Double.valueOf(Integer.valueOf(1).doubleValue() / UTIL.NumberToReal(a).doubleValue()));
  }
// ***** VDMTOOLS END Name=acot#1|Number


// ***** VDMTOOLS START Name=pre_acot#1|Number KEEP=YES
  public static Boolean pre_acot (final Number a) throws CGException {
    return Boolean.valueOf(UTIL.NumberToReal(a).doubleValue() != 0);
  }
// ***** VDMTOOLS END Name=pre_acot#1|Number


// ***** VDMTOOLS START Name=sqrt#1Number KEEP=YES
  public static Double sqrt (final Number a) throws CGException{
    return Double.valueOf(Math.sqrt(UTIL.NumberToReal(a).doubleValue()));
  }
// ***** VDMTOOLS END Name=sqrt#1|Number


// ***** VDMTOOLS START Name=pre_sqrt#1Number KEEP=YES
  public static Boolean pre_sqrt (final Number a) throws CGException {
    return Boolean.valueOf((UTIL.NumberToReal(a).doubleValue()) >= 0);
  }
// ***** VDMTOOLS END Name=pre_sqrt#1|Number


// ***** VDMTOOLS START Name=srand#1|Number KEEP=YES
  public static void srand (final Number a) throws CGException {
    MATH.rnd_seed = UTIL.NumberToInt(a).longValue();
    if( MATH.rnd_seed >= 0 ) {
      rd.set_seed(UTIL.NumberToInt(a).longValue());
    }
  }
// ***** VDMTOOLS END Name=srand#1|Number

// ***** VDMTOOLS START Name=srand2#1|Number KEEP=YES
  public static Integer srand2 (final Number a) throws CGException {
    srand(a);
    return UTIL.NumberToInt(a);
  }
// ***** VDMTOOLS END Name=srand2#1|Number

// ***** VDMTOOLS START Name=srand#1|Long KEEP=YES
  public static void srand (final Long a) throws CGException {
    MATH.rnd_seed = a.longValue();
    if( MATH.rnd_seed >= 0 ) {
      rd.set_seed(a.longValue());
    }
  }
// ***** VDMTOOLS END Name=srand#1|Long

// ***** VDMTOOLS START Name=srand2#1|Long KEEP=YES
  public static Long srand2 (final Long a) throws CGException {
    srand(a);
    return a;
  }
// ***** VDMTOOLS END Name=srand2#1|Long


// ***** VDMTOOLS START Name=pre_srand#1|Number KEEP=YES
  public static Boolean pre_srand (final Number a) throws CGException {
    return Boolean.valueOf((UTIL.NumberToInt(a).longValue()) >= -1);
  }
// ***** VDMTOOLS END Name=pre_srand#1|Number


// ***** VDMTOOLS START Name=rand#1|Number KEEP=YES
  public static Integer rand (final Number a) throws CGException {
    if( MATH.rnd_seed >= 0 ) {
      return Integer.valueOf(rd.get_random(UTIL.NumberToInt(a).intValue()));
    }
    else {
      return Integer.valueOf(UTIL.NumberToInt(a).intValue());
    }
  }
// ***** VDMTOOLS END Name=rand#1|Number

// ***** VDMTOOLS START Name=rand#1|Long KEEP=YES
  public static Long rand (final Long a) throws CGException {
    if( MATH.rnd_seed >= 0 ) {
      return Long.valueOf(rd.get_random(a.intValue()));
    }
    else {
      return Long.valueOf(a.longValue());
    }
  }
// ***** VDMTOOLS END Name=rand#1|Long


// ***** VDMTOOLS START Name=exp#1|Number KEEP=YES
  public static Double exp (final Number a) throws CGException {
    return Double.valueOf(Math.exp(UTIL.NumberToReal(a).doubleValue()));
  }
// ***** VDMTOOLS END Name=exp#1|Number


// ***** VDMTOOLS START Name=ln#1|Number KEEP=YES
  public static Double ln (final Number a) throws CGException {
    return Double.valueOf(Math.log(UTIL.NumberToReal(a).doubleValue()));
  }
// ***** VDMTOOLS END Name=ln#1|Number


// ***** VDMTOOLS START Name=pre_ln#1|Number KEEP=YES
  public static Boolean pre_ln (final Number a) throws CGException {
    return Boolean.valueOf((UTIL.NumberToReal(a).doubleValue()) > 0);
  }
// ***** VDMTOOLS END Name=pre_ln#1|Number


// ***** VDMTOOLS START Name=log#1|Number KEEP=YES
  public static Double log (final Number a) throws CGException {
  //    return new Double(Math.log10(UTIL.NumberToReal(a).doubleValue()));
    return Double.valueOf(Math.log(UTIL.NumberToReal(a).doubleValue())/Math.log(10));
  }
// ***** VDMTOOLS END Name=log#1|Number


// ***** VDMTOOLS START Name=pre_log#1|Number KEEP=YES
  public static Boolean pre_log (final Number a) throws CGException {
    return Boolean.valueOf((UTIL.NumberToReal(a).doubleValue()) > 0);
  }
// ***** VDMTOOLS END Name=pre_log#1|Number


// ***** VDMTOOLS START Name=pi_uf KEEP=YES
  public static Double pi_uf () throws CGException {
    return pi;
  }
// ***** VDMTOOLS END Name=pi_uf

// ***** VDMTOOLS START Name=pi_f KEEP=YES
  public static Double pi_f () throws CGException {
    return pi;
  }
// ***** VDMTOOLS END Name=pi_f

}
;
