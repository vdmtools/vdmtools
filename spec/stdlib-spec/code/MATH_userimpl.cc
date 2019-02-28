#include "random.h"

static Random rnd;
static long rnd_seed = -1;

#ifdef VDMPP
Real vdm_MATH::vdm_acos(const Real &r)
#endif
#ifdef VDMSL
Real vdm_MATH_acos(const Real &r)
#endif
{
  return Real (acos (r.GetValue ()));
}


#ifdef VDMPP
Bool vdm_MATH::vdm_pre_acos(const Real &r)
#endif
#ifdef VDMSL
Bool vdm_MATH_pre_acos(const Real &r)
#endif
{
  return Bool (fabs (r.GetValue ()) <= 1.0);
}


#ifdef VDMPP
Real vdm_MATH::vdm_asin(const Real &r)
#endif
#ifdef VDMSL
Real vdm_MATH_asin(const Real &r)
#endif
{
  return Real (asin (r.GetValue ()));
}


#ifdef VDMPP
Bool vdm_MATH::vdm_pre_asin(const Real &r)
#endif
#ifdef VDMSL
Bool vdm_MATH_pre_asin(const Real &r)
#endif
{
  return Bool (fabs (r.GetValue ()) <= 1.0);
}


#ifdef VDMPP
Real vdm_MATH::vdm_atan(const Real &r)
#endif
#ifdef VDMSL
Real vdm_MATH_atan(const Real &r)
#endif
{
  return Real (atan (r.GetValue ()));
}


#ifdef VDMPP
Real vdm_MATH::vdm_cos(const Real &r)
#endif
#ifdef VDMSL
Real vdm_MATH_cos(const Real &r)
#endif
{
  return Real (cos (r.GetValue ()));
}


#ifdef VDMPP
Bool vdm_MATH::vdm_post_cos(const Real &r1, const Real &r2)
#endif
#ifdef VDMSL
Bool vdm_MATH_post_cos(const Real &r1, const Real &r2)
#endif
{
  return Bool (fabs (r2.GetValue ()) <= 1.0);
}


#ifdef VDMPP
Real vdm_MATH::vdm_cot(const Real &r)
#endif
#ifdef VDMSL
Real vdm_MATH_cot(const Real &r)
#endif
{
  return Real ( 1.0/tan (r.GetValue()));
}


#ifdef VDMPP
Real vdm_MATH::vdm_pi_uf()
#endif
#ifdef VDMSL
Real vdm_MATH_pi_uf()
#endif
{
#ifdef VDMPP
  return Real (vdm_MATH::vdm_pi);
#endif VDMPP
#ifdef VDMSL
  return Real (vdm_MATH_pi);
#endif VDMSL
}


#ifdef VDMPP
Real vdm_MATH::vdm_sin(const Real &r)
#endif
#ifdef VDMSL
Real vdm_MATH_sin(const Real &r)
#endif
{
  return Real (sin (r.GetValue ()));
}


#ifdef VDMPP
Bool vdm_MATH::vdm_post_sin(const Real &r1, const Real &r2)
#endif
#ifdef VDMSL
Bool vdm_MATH_post_sin(const Real &r1, const Real &r2)
#endif
{
  return Bool (fabs (r2.GetValue ()) <= 1.0);
}


#ifdef VDMPP
Real vdm_MATH::vdm_sqrt(const Real &r)
#endif
#ifdef VDMSL
Real vdm_MATH_sqrt(const Real &r)
#endif
{
  return Real (sqrt (r.GetValue ()));
}


#ifdef VDMPP
Bool vdm_MATH::vdm_pre_sqrt(const Real &r)
#endif
#ifdef VDMSL
Bool vdm_MATH_pre_sqrt(const Real &r)
#endif
{
  return Bool (r.GetValue () >= 0.0);
}


#ifdef VDMPP
Real vdm_MATH::vdm_tan(const Real &r)
#endif
#ifdef VDMSL
Real vdm_MATH_tan(const Real &r)
#endif
{
  return Real (tan (r.GetValue ()));
}


#ifdef VDMPP
Int vdm_MATH::vdm_srand2(const Int &r)
#endif
#ifdef VDMSL
Int vdm_MATH_srand2(const Int &r)
#endif
{
  rnd_seed = r.GetValue();
  if( rnd_seed >= 0 ) {
    rnd.set_seed(r.GetValue());
  }
  return r;
}


#ifdef VDMPP
Bool vdm_MATH::vdm_pre_srand2(const Int &r)
#endif
#ifdef VDMSL
Bool vdm_MATH_pre_srand2(const Int &r)
#endif
{
  return Bool (r.GetValue () >= -1);
}


#ifdef VDMPP
Int vdm_MATH::vdm_rand(const Int &r)
#endif
#ifdef VDMSL
Int vdm_MATH_rand(const Int &r)
#endif
{
  if( rnd_seed >= 0 ) {
    return Int(rnd.get_random(r.GetValue()));
  }
  else {
    return r;
  }
}


#ifdef VDMPP
Real vdm_MATH::vdm_exp(const Real &r)
#endif
#ifdef VDMSL
Real vdm_MATH_exp(const Real &r)
#endif
{
  return Real (exp (r.GetValue ()));
}


#ifdef VDMPP
Real vdm_MATH::vdm_ln(const Real &r)
#endif
#ifdef VDMSL
Real vdm_MATH_ln(const Real &r)
#endif
{
  return Real (log (r.GetValue ()));
}


#ifdef VDMPP
Bool vdm_MATH::vdm_pre_ln(const Real &r)
#endif
#ifdef VDMSL
Bool vdm_MATH_pre_ln(const Real &r)
#endif
{
  return Bool (r.GetValue () > 0.0);
}


#ifdef VDMPP
Real vdm_MATH::vdm_log(const Real &r)
#endif
#ifdef VDMSL
Real vdm_MATH_log(const Real &r)
#endif
{
  return Real (log10 (r.GetValue ()));
}


#ifdef VDMPP
Bool vdm_MATH::vdm_pre_log(const Real &r)
#endif
#ifdef VDMSL
Bool vdm_MATH_pre_log(const Real &r)
#endif
{
  return Bool (r.GetValue () > 0.0);
}


