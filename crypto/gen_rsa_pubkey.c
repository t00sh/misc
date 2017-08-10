/*
   To compile : gcc -lcrypto gen_rsa_pubkey.c -i gen_rsa_pubkey
*/

#include <stdio.h>

#include <openssl/bn.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

int main(int argc, char **argv) {
  RSA *keypair = RSA_new();
  BIGNUM *n = BN_new();
  BIGNUM *e = BN_new();

  if(argc != 3) {
    printf("Usage : %s <n> <e>\n", argv[0]);
    return 1;
  }

  BN_dec2bn(&n, argv[1]);
  BN_dec2bn(&e, argv[2]);

  RSA_set0_key(keypair, n, e, NULL);

  PEM_write_RSAPublicKey(stdout, keypair);
  RSA_free(keypair);

  return 0;
}
