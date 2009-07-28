#ifndef INVADERS_CONFIG_H_
#define INVADERS_CONFIG_H_

#include <types.h>

const uint BoardSize = 19;
const uint PawnSize = 5;


#ifdef _MSC_VER
#define no_inline   __declspec(noinline)
#define flatten
#define all_inline  __forceinline
#else
#define no_inline   __attribute__((noinline))
#define flatten     __attribute__((flatten))
#define all_inline  __attribute__((always_inline))
#endif //_MSC_VER

#endif

