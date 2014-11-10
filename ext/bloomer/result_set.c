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

void merge_results(int** results, int* result_counts, int bucket_count, ResultSet* dest) {
  ResultSet* tmp;
  int i, j;
  int initial_size = 0;
  int last;
  for (i = 0; i < bucket_count; ++i) {
    initial_size += result_counts[i];
  }
  tmp = create_result_set(initial_size);
  for (i = 0; i < bucket_count; ++i) {
    printf("adding results: ");
    for (j = 0; j < result_counts[i]; ++j) {
      printf("%d, ", results[i][j]);
    }
    printf("\n");
    add_results(tmp, results[i], result_counts[i]);
  }
  qsort(tmp->data, tmp->count, sizeof(int), comparison);
  add_results(dest, tmp->data, 1);
  last = tmp->data[0];  
  for (i = 1; i < tmp->count; ++i) {
    if (last == tmp->data[i]) {      
      continue;
    }
    else {
      add_results(dest, tmp->data + i, 1);
      last = tmp->data[i];
    }
  }
  for (j = 0; j < dest->count; ++j) {
    printf("%d\n", dest->data[j]);
  }
  delete_result_set(tmp);  
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