#ifndef _BLOOMER_H
#define _BLOOMER_H

#include <stdint.h>

typedef struct {
  int bitwidth;  
  int intwidth;
  int** buckets;
  int* bucket_lengths;
  int* bucket_counts;
  int bucket_length_step;
  int hash_keys[4];
  int max_bucket_count;
} Bloomer;

Bloomer* create_bloomer(int, int);
void resize_bucket(Bloomer* b, int bucket);
int is_set(uint32_t* bitfield, int position);
void add_value_to_bucket(Bloomer* b, int bucket, int value, int sort);

#endif