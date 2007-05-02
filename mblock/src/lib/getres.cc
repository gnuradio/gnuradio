#include <time.h>
#include <stdio.h>

int
main(int argc, char **argv)
{
  bool ok = true;
  struct timespec ts;
  int r;

  r = clock_getres(CLOCK_REALTIME, &ts);
  if (r != 0){
    perror("clock_getres(CLOCK_REALTIME, ...)");
    ok = false;
  }
  else
    printf("clock_getres(CLOCK_REALTIME, ...)  => %11.9f\n",
	   (double) ts.tv_sec + ts.tv_nsec * 1e-9);


  r = clock_getres(CLOCK_MONOTONIC, &ts);
  if (r != 0){
    perror("clock_getres(CLOCK_MONOTONIC, ...");
    ok = false;
  }
  else
    printf("clock_getres(CLOCK_MONOTONIC, ...) => %11.9f\n",
	   (double) ts.tv_sec + ts.tv_nsec * 1e-9);


  return ok == true ? 0 : 1;
}  
