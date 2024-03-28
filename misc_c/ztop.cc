/*  ztop - convert a single z value into p

    Stephen Smith, Tom Nichols and Matthew Webster, FMRIB Image Analysis Group

    Copyright (C) 2002-2014 University of Oxford  */

/*  CCOPYRIGHT */

#include <ztop_function.h>

/* }}} */
/* {{{ usage */

using namespace ZTOP;

void usage(void)
{
  printf("Usage: ztop <z> [-2] [-g <number_of_resels>]\n");
  printf("-2 : use 2-tailed conversion (default is 1-tailed)\n");
  printf("-g : use GRF maximum-height theory instead of Gaussian pdf\n");
  exit(1);
}

/* }}} */

int main(int argc,char *argv[])
{
  int i, twotailed=0, grf=0;
  double p, z, nresels=0;

  if (argc<2)
    usage();

  z=atof(argv[1]);

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

  p = ztop_function(twotailed, grf, z, nresels);

  if (p>0.0001)
    printf("%f\n",p);
  else
    printf("%e\n",p);

  return(0);
}
