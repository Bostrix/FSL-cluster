/*  connectedcomp.cc

    Mark Jenkinson, FMRIB Image Analysis Group

    Copyright (C) 2000-2004 University of Oxford  */

/*  CCOPYRIGHT */

#include <iostream>
#include <string>
#include "newimage/newimageall.h"

using namespace std;
using namespace NEWIMAGE;

/////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
  volume<float> invol;
  volume <int> outvol;

  if (argc<2) {
    cerr << "Usage: " << argv[0] << " <in_volume> [outputvol [num_connect]]" << endl;
    return -1;
  }

  string inname = argv[1], outname;
  if (argc>2) {
    outname = argv[2];
  } else {
    outname = inname + "_label";
  }

  int num_connect=26;
  if (argc>3) {
    num_connect=atoi(argv[3]);
    if ((num_connect!=6) && (num_connect!=18) && (num_connect!=26)) {
      cerr << "Can only have num_connect equal 4, 18 or 26" << endl;
      return 1;
    }
  }

  read_volume(invol,inname);
  outvol = connected_components(invol,num_connect);
  save_volume(outvol,outname);
}
