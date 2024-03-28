/* {{{ copyright */

/*  ptoz - convert a single p value into z

    Stephen Smith, FMRIB Image Analysis Group

    Copyright (C) 1999-2001 University of Oxford  */

/*  CCOPYRIGHT */

/* }}} */
/* {{{ defines */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cprob/libprob.h"

#define MINP 1e-15

using namespace MISCMATHS;
/* }}} */
/* {{{ usage */

void usage(void)
{
  printf("Usage: ptoz <p> [-2] [-g <number_of_resels>]\n");
  printf("-2 : use 2-tailed conversion (default is 1-tailed)\n");
  printf("-g : use GRF maximum-height theory instead of Gaussian pdf\n");
  exit(1);
}

/* }}} */

int main(int argc,char *argv[])
{
  int i, twotailed=0, grf=0;
  double p, nresels=0;

  if (argc<2)
    usage();

  p=atof(argv[1]);

  /* {{{ process args */

for (i=2; i<argc; i++)
{
  if (!strcmp(argv[i], "-2"))
    twotailed=1;

  else if (!strcmp(argv[i], "-g"))
    {
      grf=1;
      i++;
      if (argc<i+1)
	{
	  printf("Error: no value given following -g\n");
	  usage();
	}
      nresels=atof(argv[i]);
    }

  else
    usage();
}

/* }}} */

  if (twotailed)
    p/=2;

  if (grf)
    p/=nresels*0.11694; /* (4ln2)^1.5 / (2pi)^2 */

  if (p<MINP) p=MINP;

  if (p>0.5)
    printf("0\n");
  else
    {
      if (grf)
	{
	  double l=2, u=100, z=0, pp;
	  while (u-l>0.001)
	    {
	      z=(u+l)/2;
	      pp=exp(-0.5*z*z)*(z*z-1);
	      if (pp<p) u=z;
	      else      l=z;
	    }
	  printf("%f\n",z);
	}
      else
	printf("%f\n",ndtri(1-p));
    }

  return(0);
}
