#include "bloomer.h"

int bitwidth = 256;

Bloomer* create_bloomer(bucket_length_step) {
  Bloomer* res;
  size_t width;
  res = malloc(sizeof(Bloomer));
  res->buckets = malloc(sizeof(int*)) * bitwidth;
  res->bucket_lengths = malloc(sizeof(int)) * bitwidth;
  res->bucket_counts = malloc(sizeof(int)) * bitwidth;
  for (int i = 0; i < bitwidth; ++i) {
    res->buckets[i] = malloc(sizeof(int) * bucket_length_step)
    res->bucket_lengths[i] = bucket_length_step
    res->bucket_counts[i] = 0;
  }
  return res;
}

void delete_bloomer(Bloomer* b) {
  free(b->data);
  free(b);
}

void resize_bloomer(bloomer* b, int new_size) {
  b->data = realloc(b->data, new_size);
}

void add_element(Bloomer* b, int* bitfield, int value) {
  for(int i = )
}

void add_value_to_bucket(Bloomer* b, int bucket, int value) {
  if (b->)
}

int* retrieve_elements(Bloomer* b, int* bitfield) {
  return (int*)0;
}

int is_set(int* bitfield, int position) {
  int field_part = position / 32;
  int field_offset = position % 32;
  return (bitfield[field_part] & (1 << field_offset))
}