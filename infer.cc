/*  infer.cc

    Mark Jenkinson, FMRIB Image Analysis Group

    Copyright (C) 2000-2004 University of Oxford  */

/*  CCOPYRIGHT */

#include <iostream>
#include <cstdlib>
#include <cmath>

#include "infer.h"
#include "cprob/libprob.h"
#include "miscmaths/miscmaths.h"

#define POSIX_SOURCE 1

#if !defined(M_PI)
#define M_PI (4 * atan(1.0))
#endif

using namespace std;

Infer::Infer(float udLh, float ut, unsigned int uV) {
  // the following bounds are checked to ensure that the exponent
  //  does not underflow, which is assumed to occur for results
  //  of less than 1e-37  => abs(t)<13.0

  // assign to copies
  dLh = udLh;
  t = ut;
  V = uV;
  if (V<=0.0) V=1.0;
  // dimensionality
  D = 3.0;
  // if (zsize <= 1)  D = 2.0;  // to be done by calling program

  // NB: the (sqr(t) -1) is previous D=3 version (from where??)
  if (fabs(t)<13.0) {
    Em_ = V * pow(double(2*M_PI),double(-(D+1)/2)) * dLh * pow((MISCMATHS::Sqr(t) - 1), (D-1)/2) *
      exp(-MISCMATHS::Sqr(t)/2.0);
  } else {
    Em_ = 0.0;  // underflowed exp()
  }

  if (fabs(t)<8.0) {
    B_ = pow((MISCMATHS::gamma(1.0+D/2.0)*Em_)/(V*(0.5 + 0.5*MISCMATHS::erf(-t/sqrt(2.0)))),(2.0/D));
  } else {
    // the large t approximation  (see appendix below)
    float a1 = V * dLh * pow(double(2*M_PI),double(-(D+1)/2));
    float a3 = pow((MISCMATHS::gamma(1+D/2.0)  / V ),(2.0/D));
    float tsq = t*t;
    float c = pow(2*M_PI,-1.0/2.0) * t / ( 1.0 - 1.0/tsq + 3.0/(tsq*tsq)) ;
    float Em_q = a1 * pow(double(tsq - 1.0),double(D-1)/2) * c;
    B_ = a3 * pow(double(Em_q),double(2.0/D));
  }


//      cout << "E{m} " << Em_ << endl;
//      cout << "Beta = " << B_ << endl;
}

//////////////////////////////////////////////////////////////////////////////

// Calculate and return log(p)

float Infer::operator() (unsigned int k) {
  // ideally returns the following:
  //    return 1 - exp(-Em_ * exp(-B_ * pow( k , 2.0 / D)));
  // but in practice must be careful about ranges
  // Assumes that exp(+/-87) => 1e+/38 is OK for floats
  float exponent_thresh = 80.0;
  float arg1 = -B_ * pow(k , 2.0 / D);
  if (fabs(arg1)>exponent_thresh) {
    // approximation for logp
    float logp = arg1 + log(Em_);
    return logp;
  } else {
    float exp1 = exp(arg1);
    float arg2 = -Em_ * exp1;
    if (fabs(arg2)>exponent_thresh) {
      // approximation of  1 - exp(arg2)
      float p = -arg2;
      if (p>0) return log(p);
    } else {
      float exp2 = exp(arg2);
      float p = 1.0 - exp2;
      if ( (p==0.0) && (arg2<0.0) ) { p = -arg2; } // approx for 1-exp2
      return log(p);
    }
  }
  cerr << "Warning: could not compute p-value accurately." << endl;
  return -500;
}



// MATHEMATICAL APPENDIX

/*

The formulas that need to be calculated are:
(1) E_m = V * dLh * (2*pi)^(-(D+1)/2) * (t^2 -1)^((D-1)/2) * exp(-t^2 /2)
(2) Beta = (Gamma(D/2+1)/V * E_m / Phi(-t) )^(2/D)
(3) p = 1 - exp( - E_m * exp(-Beta*k^(2/D)))

where Phi(-t) = Gaussian cumulant = (1/2 + 1/2*MISCMATHS::erf(-t/sqrt(2)))

These are approximated by:

(2a) Beta = (Gamma(D/2+1)/V)^(2/D) * (Em1)^(2/D) * Ct^(2/D)
where Em1 = V * dLh * (2*pi)^(-(D+1)/2) * (t^2 -1)^((D-1)/2)
      Ct = (2*pi)^(-1/2) * t / ( 1.0 - 1.0/t^2 + 3.0/t^4 )
      which approximates ( exp(-t^2 /2) / Phi(-t) )^(2/D)
      using 1/2 - 1/2*MISCMATHS::erf(t/sqrt(2)) = (2*pi)^(1/2) * exp(-t^2 /2) *
                                        (1-1/t^2+3/t^4) / t
					(this is derived in TR00MJ1)

and

(3a) log(p) = (- Beta * k^(2/D)) + log(Em)

 */
