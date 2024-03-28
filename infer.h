/*  infer.h

    Mark Jenkinson, FMRIB Image Analysis Group

    Copyright (C) 2000-2004 University of Oxford  */

/*  CCOPYRIGHT */

#if !defined(Infer_h)
#define Infer_h

// $Id$

class Infer {
public:
  Infer(float dLh, float t, unsigned int V);
  void setD(int NumDim) { D = (float) NumDim; }

  float operator() (unsigned int k);   // returns log(p)

private:
  float Em_, B_, dLh, t, V, D;
};

#endif
