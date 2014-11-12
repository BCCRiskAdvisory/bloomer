#include "hashing.h"
#include <openssl/evp.h>

const char* digests[] = {
  "sha224",
  "sha256",
  "sha384",
  "sha512",
  0
};

int initialized = 0;

void init_hashing() {
  if (initialized) { return; }
  OpenSSL_add_all_digests();
  initialized = 1;
}

HashService* create_hash_service(int required_digest_length) {
  HashService* res = malloc(sizeof(HashService));
  int i = 0;

  init_hashing();

  while (digests[i]) {
    res->digest = EVP_get_digestbyname(digests[i]);
    if (EVP_MD_size(res->digest) >= required_digest_length) {    
      res->digest_size = EVP_MD_size(res->digest);
      break;
    }
    ++i;
  }
  return res;
}

void delete_hash_service(HashService* hs) {
  free(hs);  
}

void hash_data(HashService* hs, const char* data, int data_length, unsigned char* dest_ptr) {
  EVP_MD_CTX* digest_context;        
  digest_context = EVP_MD_CTX_create();
  EVP_DigestInit_ex(digest_context, hs->digest, NULL);
  EVP_DigestUpdate(digest_context, data, data_length);
  EVP_DigestFinal(digest_context, dest_ptr, NULL);
}