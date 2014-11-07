#ifndef _BLOOMER_H
#define _BLOOMER_H

typedef struct {
  int length;
  int max_length;
  int** buckets;
  int* bucket_lengths;
  int* bucket_counts;
  int bucket_length_step;  
} Bloomer;

Bloomer* create_bloomer(int);

#endif