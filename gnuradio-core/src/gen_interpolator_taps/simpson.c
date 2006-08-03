/* -*- c -*- */
#include <math.h>
#include <stdio.h>

#define	EPS	(1.0e-5)
#define	JMAX	16

/*
 * Compute the Nth stage of refinement of an extended trapezoidal
 * rule.  FUNC is input as a pointer to a function to be integrated
 * between limits A and B.  When called with N = 1, the routine
 * returns the crudest estimate of the integral from A to B of f(x)
 * dx.  Subsequent calls with N=2,3,... (in that sequential order)
 * will improve the accuracy by adding 2**(N-2) additional interior
 * points.
 *
 * N.B., this function contains static state and IS NEITHER RENTRANT
 * NOR THREAD SAFE!
 */

double 
trapzd (double (*func)(double),
	double a, double b,
	int    n)
{
  long double		x, tnm, sum, del;
  static long double	s;
  static int	it;
  int		j;

  if (n == 1){
    it = 1;	/* # of points to add on the next call */
    s = 0.5 * (b - a) * (func(a) + func(b));
    return s;
  }
  else {
    tnm = it;
    del = (b-a)/tnm;		/* this is the spacing of the points to be added */
    x = a + 0.5*del;
    for (sum = 0.0, j = 1; j <= it; j++, x += del)
      sum += func(x);
    it *= 2;
    s = 0.5 * (s + (b-a) * sum/tnm);	/* replace s by it's refined value */
    return s;
  }
}

/*
 * Returns the integral of the function FUNC from A to B.  The
 * parameters EPS can be set to the desired fractional accuracy and
 * JMAX so that 2**(JMAX-1) is the maximum allowed number of steps.
 * Integration is performed by Simpson's rule.
 */

double
qsimp (double (*func)(double),
       double a,	/* lower limit */
       double b)	/* upper limit */
{
  int		j;
  long double	s, st, ost, os;

  ost = os = -1.0e30;
  for (j = 1; j <= JMAX; j++){
    st = trapzd (func, a, b, j);
    s = (4.0 * st - ost)/3.0;
    if (fabs (s - os) < EPS * fabs(os))
      return s;
    os = s;
    ost = st;
  }
  fprintf (stderr, "Too many steps in routine QSIMP\n");
  // exit (1);
  return s;
}

