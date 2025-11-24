#include <sys/time.h>
#include <stdlib.h>

long currentTimeMillis() {
  struct timeval time;
  gettimeofday(&time, NULL);

  return time.tv_sec * 1000 + time.tv_usec / 1000;
}

/**
    Returns a n, where 0 <= n < numFrames. n changes fps times per second
*/
int getFrame(int fps, int numFrames) {
    #define MS_IN_SECONDS 1000
    long timeNow = currentTimeMillis();
    return  (unsigned long)(timeNow * ((double)fps/MS_IN_SECONDS) ) % numFrames;
}