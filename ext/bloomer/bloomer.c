#include "bloomer.h"
#include "result_set.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>

void populate_result_set(Bloomer* b, ResultSet* rs, int* matching_buckets, int count);

Bloomer* create_bloomer(int bitwidth, int bucket_length_step) {
  Bloomer* res;
  int i;
  res = malloc(sizeof(Bloomer));
  res->bitwidth = bitwidth;
  res->intwidth = ((bitwidth - 1) / 32) + 1;
  res->buckets = malloc(sizeof(int*) * bitwidth);
  res->bucket_lengths = malloc(sizeof(int) * bitwidth);
  res->bucket_counts = malloc(sizeof(int) * bitwidth);
  res->bucket_length_step = bucket_length_step;
  res->max_bucket_count = bucket_length_step;
  for (i = 0; i < bitwidth; ++i) {
    res->buckets[i] = malloc(sizeof(int) * bucket_length_step);
    res->bucket_lengths[i] = bucket_length_step;
    res->bucket_counts[i] = 0;
  }
  return res;
}

void delete_bloomer(Bloomer* b) {
  int i;
  free(b->bucket_lengths);
  free(b->bucket_counts);
  for (i = 0; i < b->bitwidth; ++i) {
    free(b->buckets[i]);    
  }
  free(b->buckets);
  free(b);
}

void print_bloomer(Bloomer* b) {
  int i, j;
  printf("%d Buckets in total\n\n", b->bitwidth);
  for(i = 0; i < b->bitwidth; ++i) {
    printf("%d", b->bucket_counts[i]);
    /*printf("Bucket %d, length: %d, count: %d\n\n", i, b->bucket_lengths[i], b->bucket_counts[i]);    
    for (j = 0; j < b->bucket_counts[i]; ++j) {
      printf("%d\n", b->buckets[i][j]);
    }
    */
  }
  printf("\n");
}

void add_element(Bloomer* b, uint32_t* bitfield, int value) {
  int i;
  for (i = 0; i < b->intwidth; ++i) {
    printf("%d: %d\n", i, bitfield[i]);
  }
  printf("adding ");
  for(i = 0; i < b->bitwidth; ++i) {
    if (is_set(bitfield, i)) {
      printf("1");
      add_value_to_bucket(b, i, value, 1);
    }
    else {
      printf("0");
    }
  }
  printf("\n");
}

void add_value_to_bucket(Bloomer* b, int bucket, int value, int sort) {
  if (b->bucket_counts[bucket] >= b->bucket_lengths[bucket]) {
    printf("Resizing bucket %d\n", bucket);
    resize_bucket(b, bucket);
  }
  b->buckets[bucket][b->bucket_counts[bucket]] = value;
  ++b->bucket_counts[bucket];
  if (sort) {
    qsort(b->buckets[bucket], b->bucket_counts[bucket], sizeof(int), comparison);
  }
}

void sort_buckets(Bloomer* b) {
  int bucket = 0;
  for (bucket = 0; bucket < b->bitwidth; ++bucket) {
    qsort(b->buckets[bucket], b->bucket_counts[bucket], sizeof(int), comparison);
  }
}

void resize_bucket(Bloomer* b, int bucket) {
  int new_size = b->bucket_lengths[bucket] + b->bucket_length_step;
  b->bucket_lengths[bucket] = new_size;
  b->buckets[bucket] = realloc(b->buckets[bucket], sizeof(int) * new_size);
  if (new_size > b->max_bucket_count) {
    b->max_bucket_count = new_size;
  }
}

ResultSet* retrieve_elements(Bloomer* b, uint32_t* bitfield) {
  int* matching_buckets;
  int i, count;
  ResultSet* res;

  count = 0;  
  matching_buckets = malloc(sizeof(int*) * b->bitwidth);
  for (i = 0; i < b->bitwidth; ++i) {
    if (is_set(bitfield, i)) {
      matching_buckets[count] = i;
      ++count;
    }
  }
  res = create_result_set(b->max_bucket_count * 2);
  populate_result_set(b, res, matching_buckets, count);
  free(matching_buckets);
  return res;
}

void populate_result_set(Bloomer* b, ResultSet* rs, int* matching_buckets, int count) {
  int i, j;
  int** data;
  int* counts;
  data = malloc(sizeof(int*) * count);
  counts = malloc(sizeof(int) * count);
  for (i = 0; i < count; ++i) {
    printf("bucket %d matches\n", matching_buckets[i]);
    data[i] = b->buckets[matching_buckets[i]];
    counts[i] = b->bucket_counts[matching_buckets[i]];
  }
  merge_results(data, counts, count, rs);  
}

int is_set(uint32_t* bitfield, int position) {
  int field_part = position / 32;
  int field_offset = position % 32;
  return (bitfield[field_part] & (1 << field_offset));
}