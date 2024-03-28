/* {{{ Copyright etc. */

/*  hist2prob - convert histogram into probability values

    Stephen Smith, FMRIB Image Analysis Group

    Copyright (C) 1999-2000 University of Oxford  */

/*  CCOPYRIGHT */


/* }}} */
/* {{{ includes and defines */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

/* }}} */
/* {{{ main() */

int main(argc,argv)
  int argc;
  char *argv[];
{
/* {{{ variables */

FILE   *ifp;
int    i, image_size, count, n_points=1000, total;
float  *hist_data,low_thresh,high_thresh,incr,tmpf;

/* }}} */

  /* {{{ setup files, etc */

  /*printf("hist2prob by Steve Smith\n");*/

  if (argc<5)
  {
    printf("Usage: hist2prob <image> <size> <low_thresh> <high_thresh>\n");
    exit(1);
  }

  if ((ifp=fopen(argv[1],"rb")) == NULL)
  {
    printf("File %s isn't readable\n",argv[1]);
    exit(-1);
  }
  image_size=atoi(argv[2]);
  low_thresh=atof(argv[3]);
  high_thresh=atof(argv[4]);
  incr=(high_thresh-low_thresh)/(float)n_points;
  hist_data=(float*)malloc(image_size*sizeof(float));
  fread(hist_data,sizeof(float),image_size,ifp);

  count=0;
  for(i=0; i<image_size; i++)
    if ( (hist_data[i]>low_thresh) && (hist_data[i]<high_thresh) )
      count++;

  /*printf("count=%d\n",count);*/

  for(tmpf=high_thresh;tmpf>low_thresh;tmpf-=incr)
  {
    total=0;
    for(i=0; i<image_size; i++)
      if ( (hist_data[i]>tmpf) && (hist_data[i]<high_thresh) )
        total++;
    total /= 2; /* this is to make it a one-tailed test? */
    printf("%f %f\n",tmpf,log10(((float)total)/((float)count)));
  }

/* }}} */
  return (0);
}

/* }}} */
