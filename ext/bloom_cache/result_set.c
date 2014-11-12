#include "result_set.h"
#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void delete_result_set(ResultSet* r) {
  free(r->data);
  free(r);
}

ResultSet* create_result_set(int initial_size) {
  ResultSet* res;
  res = malloc(sizeof(ResultSet));
  res->data = malloc(initial_size * sizeof(int));
  res->size = initial_size;
  res->size_step = initial_size / 2;
  res->count = 0;  
  return res;
}

ResultSet* intersection(int* set1, int* set2, int count1, int count2, ResultSet* dest) {
  ResultSet* res;
  int i = 0;
  int j = 0;

  if (dest == 0) {
    res = create_result_set(count1 / 4);
  }
  else {
    dest->count = 0;
    res = dest;
  }

  while(i < count1 && j < count2) {
    if (set1[i] > set2[j]) {
      ++j;
    }
    else if (set1[i] < set2[j]) {
      ++i;
    }
    else {      
      add_results(res, (set1 + i), 1);
      ++i;
      ++j;
    }
  }
  return res;
}

void merge_results(int** results, int* result_counts, int bucket_count, ResultSet* dest) {
  ResultSet* tmp[2];
  int i;

  if (bucket_count == 1) {
    add_results(dest, results[0], result_counts[0]);
    return;
  }
  
  tmp[0] = intersection(results[0], results[1], result_counts[0], result_counts[1], NULL);

  if (bucket_count == 2) {
    add_results(dest, tmp[0]->data, tmp[0]->count);
    return;
  }

  tmp[1] = intersection(tmp[0]->data, results[2], tmp[0]->count, result_counts[2], NULL);  

  for (i = 2; i < bucket_count - 1; ++i) {
    intersection(tmp[i % 2]->data, results[i + 1], tmp[i % 2]->count, result_counts[i + 1], tmp[(i + 1) % 2]);
  }
  
  add_results(dest, tmp[i % 2]->data, tmp[i % 2]->count);
  
  delete_result_set(tmp[0]);  
  delete_result_set(tmp[1]);
}

void add_results(ResultSet* rs, int* results, int result_count) {
  if (result_count == 0) { return; }

  if (rs->count + result_count >= rs->size) {
    resize_result_set(rs, rs->count + result_count + rs->size_step);
  }  
  memcpy(rs->data + rs->count, results, result_count * sizeof(int));  
  rs->count += result_count;
}

void resize_result_set(ResultSet* rs, int count) {
  int new_size;
  new_size = rs->size + count;
  rs->data = realloc(rs->data, new_size * sizeof(int));
  rs->size = new_size;
}