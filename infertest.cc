/*  infertest.cc

    Mark Jenkinson, FMRIB Image Analysis Group

    Copyright (C) 2000-2002 University of Oxford  */

/*  CCOPYRIGHT */

#include <cstdlib>
#include <iostream>
#include <cmath>

#include "infer.h"

#include "cprob/libprob.h"

#define USAGE "usage: infer <dL> <t> <V> <k1> [<k2> <k3>...<kN>]"

int main(unsigned int argc, char **argv)
{
  if ( argc < 6 )
    {
      cerr << USAGE << endl;
      return EXIT_FAILURE;
    }

  argc--;
  float dLh = atof(argv[1]); argc--;
  float t = atof(argv[2]); argc--;
  unsigned int V = atoi(argv[3]); argc--;

  Infer infer(dLh, t, V);

  for ( unsigned int n = 0; n < argc; n++ ) {
    unsigned int k = atoi(argv[4 + n]);
    float p = infer(k);
    cout << p << " " << -log10(p) << endl;
  }
  return EXIT_SUCCESS;
}
