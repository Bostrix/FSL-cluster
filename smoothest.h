/*  smoothest.h

    Mark Jenkinson, FMRIB Image Analysis Group

    Copyright (C) 2000-2002 University of Oxford  */

/*  CCOPYRIGHT */

#if !defined(smoothest_h)
#define smoothest_h

#include "newimage/newimageall.h"

int smoothest(float &dlh, unsigned long& mask_volume, float &resels,
	      const NEWIMAGE::volume4D<float>& vin,
	      const NEWIMAGE::volume<float>& mask,
              float dof, const std::string& args);


#endif
