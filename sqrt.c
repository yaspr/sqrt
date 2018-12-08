#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <emmintrin.h>

#include "rdtsc.h"

#define META 11
#define ITER 10000

//
float rsqrt_00(float number)
{
  long i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  y  = number;
  i  = * (long *) &y;                       // evil floating point bit level hacking
  i  = 0x5f3759df - (i >> 1);               // what the fuck? 
  y  = *(float *) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
  // y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

  return y;
}

//
float rsqrt_01(float number)
{
  long i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  y  = number;
  i  = * (long *) &y;                       // evil floating point bit level hacking
  i  = 0x5f375a86 - (i >> 1);               // what the fuck? 
  y  = *(float *) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
  // y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

  return y;
}

//
float rsqrt_1(float number)
{ return 1 / sqrt(number); }

//
float rsqrt_2(float number)
{
  __m128 xvec = _mm_set_ss(number);

  float x = 1.0 / _mm_cvtss_f32(_mm_sqrt_ss(xvec));

  return x;
}

//
float rsqrt_3(float number)
{
  __m128 xvec = _mm_set_ss(number);

  float x = _mm_cvtss_f32(_mm_rsqrt_ss(xvec));

  return x;
}

//
int main(int argc, char **argv)
{
  register float r = 0.0;
  register double start, stop;
  
  printf("RSQRT0(Quake)\n");
  for (uint64_t m = 0; m < META; m++)
    {
      start = rdtsc();
      
      for (uint64_t i = 0; i < ITER; i++)
	r = rsqrt_00(4);

      stop = rdtsc();

      printf("rsqrt\t:\t%.10lf\tcycles\t:\t%lf\n", r, (double)(stop - start) / ITER);
    }
  
  printf("RSQRT0(Lomont)\n");
  for (uint64_t m = 0; m < META; m++)
    {
      start = rdtsc();
      
      for (uint64_t i = 0; i < ITER; i++)
	r = rsqrt_01(4);

      stop = rdtsc();

      printf("rsqrt\t:\t%.10lf\tcycles\t:\t%lf\n", r, (double)(stop - start) / ITER);
   }

  printf("RSQRT1(1/sqrt)\n");
  for (uint64_t m = 0; m < META; m++)
    {
      start = rdtsc();
      
      for (uint64_t i = 0; i < ITER; i++)
	r = rsqrt_1(4);

      stop = rdtsc();

      printf("rsqrt\t:\t%.10lf\tcycles\t:\t%lf\n", r, (double)(stop - start) / ITER);
    }
    
  printf("RSQRT2(sqrtss)\n");
  for (uint64_t m = 0; m < META; m++)
    {
      start = rdtsc();
      
      for (uint64_t i = 0; i < ITER; i++)
	r = rsqrt_2(4);

      stop = rdtsc();

      printf("rsqrt\t:\t%.10lf\tcycles\t:\t%lf\n", r, (double)(stop - start) / ITER);
    }

  printf("RSQRT3(rsqrtss)\n");
  for (uint64_t m = 0; m < META; m++)
    {
      start = rdtsc();
      
      for (uint64_t i = 0; i < ITER; i++)
	r = rsqrt_2(4);

      stop = rdtsc();

      printf("rsqrt\t:\t%.10lf\tcycles\t:\t%lf\n", r, (double)(stop - start) / ITER);
    }

  return 0;
}
