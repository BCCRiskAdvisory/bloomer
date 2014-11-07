#ifndef _BLOOMER_H
#define _BLOOMER_H

typedef struct {
  int length;
  int max_length;
  int** buckets;
  int* bucket_lengths;
  int* bucket_counts;
  int filter_byte_width;  
} Bloomer;

Bloomer* create_bloomer(int);

#endif