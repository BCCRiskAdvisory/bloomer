#ifndef _RESULT_SET_H
#define _RESULT_SET_H

typedef struct {
  int* data;
  int count;
  int size;
  int size_step;
} ResultSet;

void delete_result_set(ResultSet* r);
ResultSet* create_result_set(int initial_size);
void add_result(ResultSet* rs, int result);
void add_results(ResultSet* rs, int* results, int result_count);
void resize_result_set(ResultSet* rs, int count);
void merge_results(int** results, int* result_counts, int bucket_count, ResultSet* dest);

#endif