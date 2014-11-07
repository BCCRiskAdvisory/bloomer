#include "bloomer.h"
#include <stdlib.h>

int bitwidth = 256;

Bloomer* create_bloomer(bucket_length_step) {
  Bloomer* res;
  size_t width;
  res = malloc(sizeof(Bloomer));
  res->buckets = malloc(sizeof(int*)) * bitwidth;
  res->bucket_lengths = malloc(sizeof(int)) * bitwidth;
  res->bucket_counts = malloc(sizeof(int)) * bitwidth;
  res->bucket_length_step = bucket_length_step
  for (int i = 0; i < bitwidth; ++i) {
    res->buckets[i] = malloc(sizeof(int) * bucket_length_step * 8)
    res->bucket_lengths[i] = bucket_length_step
    res->bucket_counts[i] = 0;
  }
  return res;
}

int comparison(const void* i1, const void* i2) {
  return (const int*)i1 - (const int*)i2;
}

void delete_bloomer(Bloomer* b) {
  free(b->data);
  free(b);
}

void resize_bloomer(bloomer* b, int new_size) {
  b->data = realloc(b->data, new_size);
}

void add_element(Bloomer* b, unsigned int* bitfield, int value) {
  for(int i = )
}

void add_value_to_bucket(Bloomer* b, int i, int value) {
  if (b->bucket_counts[i] == b->bucket_lengths[i]) {
    resize_bucket(b, i);
  }
  b->buckets[b->bucket_counts[i]] = value;
  qsort(b->buckets[i], b->bucket_counts[i], sizeof(int), comparison);
}

void resize_bucket(Bloomer* b, int bucket) {
  b->buckets[bucket] = realloc(b->buckets[bucket], sizeof(int) * b->bucket_lengths[bucket] + b->bucket_length_step)
}

int* retrieve_elements(Bloomer* b, unsigned int* bitfield) {
  return (int*)0;
}

int is_set(unsigned int* bitfield, int position) {
  int field_part = position / 32;
  int field_offset = position % 32;
  return (bitfield[field_part] & (1 << field_offset))
}