/*
 * Try to guess the required alignment for various types
 *
 * Can be used for ALIGNMENT_OF_SIZE() macro for new ports.
 * Note that the macro defaults to the identical mapping.
 *
 * Written by Godmar Back <gback@cs.utah.edu>
 */
#include <stdio.h>

#define A(type, name) \
struct {        \
  unsigned char _;      \
  type   x;     \
} name

A(int, Int);
A(short, Short);
A(long long, LongLong);
A(float, Float);
A(double, Double);
A(char, Char);

main()
{
#define P(name) \
  printf("alignment of %10s with size %d -> %d\n", \
        #name, sizeof(name.x), (char*)&name.x - (char*)&name)
  P(Char);
  P(Short);
  P(Int);
  P(Float);
  P(LongLong);
  P(Double);
}
