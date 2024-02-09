#ifndef SIMPLECACHE_H
#define SIMPLECACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Cache.h"

#define L2_ASSOCIATIVITY 2

void resetTime();

uint32_t getTime();

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t, uint8_t *, uint32_t);

/*********************** Cache *************************/

void initCache();
void accessL1(uint32_t, uint8_t *, uint32_t);
void accessL2(uint32_t, uint8_t *, uint32_t);

typedef struct CacheLine {
  uint8_t Valid;
  uint8_t Dirty;
  uint32_t Tag;
  uint32_t Time;
} CacheLine;

typedef struct Cache_L1 {
  CacheLine line[L1_LINES];
} Cache_L1;

typedef struct CacheSet {
    CacheLine sets[L2_ASSOCIATIVITY];
} CacheSet;

typedef struct Cache_L2 {
    CacheSet lines[L2_LINES];
} Cache_L2;

/*********************** Interfaces *************************/

void read(uint32_t, uint8_t *);

void write(uint32_t, uint8_t *);

#endif
