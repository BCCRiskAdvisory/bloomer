#include "bloom_cache.h"
#include "result_set.h"
#include "common.h"
#include "hashing.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void populate_result_set(BloomCache* b, ResultSet* rs, int* matching_buckets, int count);
void set_bit(uint32_t* bitfield, int position);
void add_element(BloomCache* b, uint32_t* bitfield, int value, int sort);

void Init_bloom_cache() {

}

void clear_cache(BloomCache* b) {
  int i;
  for (i = 0; i < b->bitwidth; ++i) {
    b->bucket_counts[i] = 0;
  }
}

BloomCache* create_bloom_cache(int bitwidth, int bucket_length_step, int hash_count) {
  BloomCache* res;
  int i;
  res = malloc(sizeof(BloomCache));  
  res->intwidth = ((bitwidth - 1) / 32) + 1;
  res->bitwidth = bitwidth;
  res->buckets = malloc(sizeof(int*) * res->bitwidth);
  res->bucket_lengths = malloc(sizeof(int) * res->bitwidth);
  res->bucket_counts = malloc(sizeof(int) * res->bitwidth);
  res->bucket_length_step = bucket_length_step;
  res->max_bucket_count = bucket_length_step;
  res->hash_count = hash_count;
  res->hash_service = create_hash_service(res->hash_count * 4);
  for (i = 0; i < bitwidth; ++i) {
    res->buckets[i] = malloc(sizeof(int) * bucket_length_step);
    res->bucket_lengths[i] = bucket_length_step;
    res->bucket_counts[i] = 0;
  }
  return res;
}

void delete_bloom_cache(BloomCache* b) {
  int i;
  delete_hash_service(b->hash_service);
  free(b->bucket_lengths);
  free(b->bucket_counts);
  for (i = 0; i < b->bitwidth; ++i) {
    free(b->buckets[i]);    
  }
  free(b->buckets);
  free(b);
}

void print_bloom_cache(BloomCache* b) {
  int i;
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

void bitfield_from_string(BloomCache* b, const char* str, int data_length, uint32_t* bitfield) {
  int i, offset;
  unsigned char* res;

  res = malloc(sizeof(char) * b->hash_service->digest_size);
  memset(bitfield, 0, sizeof(uint32_t) * b->intwidth);
  if (data_length < 3) {
    for (offset = 0; offset < data_length; ++offset) {
      hash_data(b->hash_service, str + offset, 1, res);
      for (i = 0; i < b->hash_count; ++i) {
        set_bit(bitfield, *(unsigned int*)(res + (i * 4)) % b->bitwidth);
      }
    }
  }
  else {
    for (offset = 0; offset < data_length - 2; ++offset) {
      hash_data(b->hash_service, str + offset, 3, res);
      for (i = 0; i < b->hash_count; ++i) {
        set_bit(bitfield, *(unsigned int*)(res + (i * 4)) % b->bitwidth);
      }
    }
  }
    
  free(res);
}

void add_value(BloomCache* b, const char* data, int data_length, int id, int sort) {
  uint32_t* bitfield;
  
  bitfield = malloc(sizeof(uint32_t) * b->intwidth);
  bitfield_from_string(b, data, data_length, bitfield);
  add_element(b, bitfield, id, sort);
  free(bitfield);
}

void add_element(BloomCache* b, uint32_t* bitfield, int value, int sort) {
  int i;
  for(i = 0; i < b->bitwidth; ++i) {
    if (is_set(bitfield, i)) {
      add_value_to_bucket(b, i, value, sort);      
    }    
  }
}

void add_value_to_bucket(BloomCache* b, int bucket, int value, int sort) {
  if (b->bucket_counts[bucket] >= b->bucket_lengths[bucket]) {
    resize_bucket(b, bucket);
  }
  b->buckets[bucket][b->bucket_counts[bucket]] = value;
  ++b->bucket_counts[bucket];
  if (sort) {
    qsort(b->buckets[bucket], b->bucket_counts[bucket], sizeof(int), comparison);
  }
}

void sort_buckets(BloomCache* b) {
  int bucket = 0;
  for (bucket = 0; bucket < b->bitwidth; ++bucket) {
    qsort(b->buckets[bucket], b->bucket_counts[bucket], sizeof(int), comparison);
  }
}

void resize_bucket(BloomCache* b, int bucket) {
  int new_size = b->bucket_lengths[bucket] + b->bucket_length_step;
  b->bucket_lengths[bucket] = new_size;
  b->buckets[bucket] = realloc(b->buckets[bucket], sizeof(int) * new_size);
  if (new_size > b->max_bucket_count) {
    b->max_bucket_count = new_size;
  }
}

ResultSet* retrieve_elements(BloomCache* b, uint32_t* bitfield) {
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

ResultSet* match_elements(BloomCache* b, const char* value, int data_length) {
  uint32_t* bitfield;
  //int i;
  ResultSet* res;
  bitfield = malloc(sizeof(uint32_t) * b->intwidth);
  bitfield_from_string(b, value, data_length, bitfield);
  res = retrieve_elements(b, bitfield);
  free(bitfield);
  return res;
}

void populate_result_set(BloomCache* b, ResultSet* rs, int* matching_buckets, int count) {
  int i;
  int** data;
  int* counts;
  data = malloc(sizeof(int*) * count);
  counts = malloc(sizeof(int) * count);
  for (i = 0; i < count; ++i) {
    data[i] = b->buckets[matching_buckets[i]];
    counts[i] = b->bucket_counts[matching_buckets[i]];
  }
  merge_results(data, counts, count, rs);  
}

int is_set(uint32_t* bitfield, int position) {
  int field_part = position / 32;
  int field_offset = position % 32;
  return (bitfield[field_part] & (1 << field_offset)) >> field_offset;
}

void set_bit(uint32_t* bitfield, int position) {
  int field_part = position / 32;
  int field_offset = position % 32;
  bitfield[field_part] |= (1 << field_offset);
}