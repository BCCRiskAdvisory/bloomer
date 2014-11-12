#ifndef _BLOOMER_H
#define _BLOOMER_H

#include <stdint.h>
#include "hashing.h"

typedef struct {
  int bitwidth;  
  int intwidth;
  int** buckets;
  int* bucket_lengths;
  int* bucket_counts;
  int bucket_length_step;  
  int max_bucket_count;
  int hash_count;
  HashService* hash_service;
} BloomCache;

BloomCache* create_bloom_cache(int, int, int);
void resize_bucket(BloomCache* b, int bucket);
int is_set(uint32_t* bitfield, int position);
void add_value_to_bucket(BloomCache* b, int bucket, int value, int sort);

#endif