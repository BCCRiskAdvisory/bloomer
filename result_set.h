#ifndef _RESULT_SET_H
#define _RESULT_SET_H

typedef struct {
  int* data;
  int count;
} ResultSet;

void delete_result_set(ResultSet* r) {
  free(r->data);
}

#endif