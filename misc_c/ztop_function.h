/*  ztop_function - convert a single z value into p

    Stephen Smith, Tom Nichols and Matthew Webster, Camille Maumet,
    FMRIB Image Analysis Group

    Copyright (C) 2002-2014 University of Oxford  */

/*  CCOPYRIGHT */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <algorithm>
#include "cprob/libprob.h"


#define MINP 1e-15

namespace ZTOP {

  double ztop_function(int twotailed, int grf, double z, double nresels)
  {
    double p;

    if (twotailed)
      z=std::fabs(z);

    if (grf) {
      if (z<2)
        p=1; /* Below z of 2 E(EC) becomes non-monotonic */
      else
        p = nresels * 0.11694 * exp(-0.5*z*z)*(z*z-1);  /* 0.11694 = (4ln2)^1.5 / (2pi)^2 */
    }  else {
      p=1-CPROB::ndtr(z);
    }

    if (twotailed)
      p*=2;

    p=std::min(p,1.0);

    return(p);
  }

}
