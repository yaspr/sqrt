#include <gmp.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <emmintrin.h>

#include "types.h"
#include "../ynotif/ynotif.h"
#include "../ydata/ydata.h"

#define MAX_SAMPLES 33

static inline f64 timer_nanos()
{
  struct timespec t;
  
  clock_gettime(CLOCK_MONOTONIC_RAW, &t);
  
  return ((f64)t.tv_sec * 1e9 + (f64)t.tv_nsec);
}

//Original constant
float rsqrt_quake(float number)
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

//Lomont constant
float rsqrt_lomont(float number)
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

f32 sqrt_C(f32 number)
{ return sqrtf(number); }

f32 sqrt_SSE(f32 number)
{
  __m128 xvec = _mm_set_ss(number);
  
  f32 x = _mm_cvtss_f32(_mm_sqrt_ss(xvec));

  return x;
}

f32 sqrt_SSE_inv(f32 number)
{
  __m128 xvec = _mm_set_ss(number);
  
  f32 x = 1.0 / _mm_cvtss_f32(_mm_rsqrt_ss(xvec));

  return x;
}

float sqrt_quake(f32 number)
{ return 1.0 / rsqrt_quake(number); }

float sqrt_lomont(f32 number)
{ return 1.0 / rsqrt_lomont(number); }

void benchmark_sqrt(const ascii *title, f32 (*kernel)(f32), f32 c, f64 x, u64 r)
{
  f64 e = 0.0;
  f64 v = 0.0;
  f64 t0 = 0.0;
  f64 t1 = 0.0;
  ydata_t *d = ydata_create(title, MAX_SAMPLES);
  
  for (u64 i = 0; i < d->n_datapoints; i++)
    {
      do
	{
	  t0 = timer_nanos();
	  
	  for (u64 j = 0; j < r; j++)
	    v = kernel(c);
	  
	  t1 = timer_nanos();
	  
	  e = (t1 - t0) / (f64)r;
	}
      while (e <= 0.0);
      
      d->datapoints[i] = e;
    }
  
  ydata_analyze(d);
  printf("v = %15.10f; d = %15.10lf ==> ", v, fabs(x - v));
  ydata_print(d);
  
  ydata_destroy(&d);
}

i32 main(i32 argc, ascii **argv)
{
  if (argc < 3)
    return printf("Usage: %s [c] [r]\n", argv[0]), 1;
  
  f32 c = (f32)atof(argv[1]);
  u64 r = atoll(argv[2]);

  //Reference values

  //64-bit 
  f64 x = sqrt((f64)c);

  //80-bit
  long double y = sqrtl(strtold(argv[1], NULL)); 

  //Arbitrary precision
  mpf_t a;
  mpf_t b;
  
  mpf_set_default_prec(1000);
  mpf_init(a);
  mpf_init(b);
  mpf_set_str(a, argv[1], 10);
  mpf_sqrt(b, a);

  printf("Reference values:\n\t%.10lf (64-bit)\n\t%.17Lf (80-bit)\n", x, y);
  gmp_printf("\t%.100Ff (1000-bit)\n\n", b);
  
  benchmark_sqrt("Quake   ", sqrt_quake  , c, x, r);
  benchmark_sqrt("Lomont  ", sqrt_lomont , c, x, r);
  benchmark_sqrt("C       ", sqrt_C      , c, x, r);
  benchmark_sqrt("SSE     ", sqrt_SSE    , c, x, r);
  benchmark_sqrt("SSE inv.", sqrt_SSE_inv, c, x, r);
  
  return 0;
}
