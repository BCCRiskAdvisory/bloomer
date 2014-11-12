#ifndef _HASHING_H
#define _HASHING_H

#include <openssl/evp.h>

typedef struct {
  const EVP_MD* digest;
  size_t digest_size;
} HashService;

void hash_data(HashService* hs, const char* data, int data_length, unsigned char* dest_ptr);
HashService* create_hash_service(int required_digest_length);
void delete_hash_service(HashService* hs);

#endif