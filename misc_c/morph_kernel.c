/* {{{ Copyright etc. */

/*  morph_kernel - create 3D 0/1 ASCII morphology kernels in a very proper way

    Stephen Smith, FMRIB Image Analysis Group

    Copyright (C) 1999-2000 University of Oxford  */

/*  CCOPYRIGHT */

/* }}} */
/* {{{ includes and defines */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* }}} */

int main(argc,argv)
  int argc;
  char *argv[];
{
  /* {{{ variables */

float a,b,c,d,r,delta=0.05;
int   w,x,y,z;

/* }}} */

  /* {{{ usage and read arg */

if (argc<3)
{
  printf("Usage: morph_kernel <cube_side_length> <sphere_radius>\n");
  printf("e.g.: morph_kernel 11 5.5 > sphere111111.ker\n");
  exit(1);
}

w=atoi(argv[1]);
r=atof(argv[2]);

d=(w-1.0)/2;
r+=0.1; /* this so that x.5 values do actually include x.5 voxels */
r=r*r;   /* this to save having to do a sqrt */

/* }}} */
  /* {{{ print header */

printf("%%!XC-Volume\n\n/PixelEncoding  /Unsigned       def\n/Width  %d       def\n/Height %d       def\n/Slices %d       def\n/Depth  8       def\n\nBeginVolumeSamples\n\n",w,w,w);

/* }}} */
  /* {{{ output 0s and 1s */

for (z=0; z<w; z++)
{
  for (y=0; y<w; y++)
    {
      for (x=0; x<w; x++)
	{
	  int count=0, sum=0;

	  for (c=-d-0.5; c<-d+0.5+delta/2; c+=delta)
	    for (b=-d-0.5; b<-d+0.5+delta/2; b+=delta)
	      for (a=-d-0.5; a<-d+0.5+delta/2; a+=delta)
		{
		  count++;
		  if ( (z+c)*(z+c) + (y+b)*(y+b) + (x+a)*(x+a) <= r )
		    sum++;
		}

	  printf("%d ",(int)(sum>(int)(count/2)));
	}
      printf("\n");
    }

  printf("\n");
}

/* }}} */
  /* {{{ print footer */

  printf("EndVolumeSamples\n");

/* }}} */

  return 0;
}
