/*
   code from http://repo.zenk-security.com/Cryptographie%20.%20Algorithmes%20.%20Steganographie/Attaque%20par%20factorisation%20contre%20RSA.pdf

   To compile : gcc -lssl -lcrypto gen_privkey.c -o gen_privkey
*/


#include <stdio.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/pem.h>


int main(int argc, char **argv) {
  RSA *keypair = RSA_new();
  BN_CTX *ctx = BN_CTX_new();
  BN_CTX_start(ctx);

  BIGNUM *n = BN_new();
  BIGNUM *d = BN_new();
  BIGNUM *e = BN_new();
  BIGNUM *p = BN_new();
  BIGNUM *q = BN_new();
  BIGNUM *dmp1 = BN_new();
  BIGNUM *dmq1 = BN_new();
  BIGNUM *iqmp = BN_new();
  BIGNUM *r0 = BN_CTX_get(ctx);
  BIGNUM *r1 = BN_CTX_get(ctx);
  BIGNUM *r2 = BN_CTX_get(ctx);
  BIGNUM *r3 = BN_CTX_get(ctx);

  if(argc != 4) {
    printf("Usage : %s <p> <q> <e>\n", argv[0]);
    return 1;
  }

  BN_dec2bn(&p, argv[1]);
  BN_dec2bn(&q, argv[2]);
  BN_dec2bn(&e, argv[3]);

  if(BN_cmp(p,q) < 0) {
    BIGNUM *tmp = p;

    p = q;
    q = tmp;
  }

  BN_mul(n,p,q,ctx);
  BN_sub(r1,p,BN_value_one());
  BN_sub(r2,q,BN_value_one());
  BN_mul(r0,r1,r2,ctx);
  BN_mod_inverse(d,e,r0,ctx);
  BN_mod(dmp1, d, r1, ctx);
  BN_mod(dmp1, d, r2, ctx);

  BN_mod_inverse(iqmp,q,p,ctx);

  keypair->n = n;
  keypair->d = d;
  keypair->e = e;
  keypair->p = p;
  keypair->q = q;
  keypair->dmq1 = dmq1;
  keypair->dmp1 = dmp1;
  keypair->iqmp = iqmp;

  PEM_write_RSAPrivateKey(stdout, keypair, NULL, NULL, 0, NULL, NULL);
  PEM_write_RSAPublicKey(stdout, keypair);
  BN_CTX_end(ctx);
  BN_CTX_free(ctx);
  RSA_free(keypair);

  return 0;
}
