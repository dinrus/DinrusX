/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 */

/**
    @file eax_decrypt.c
    EAX implementation, decrypt block, by Tom St Denis
*/
#include <DinrusX/3rdParty/tomcrypt/tomcrypt.h>

#ifdef LTC_EAX_MODE

/**
   Decrypt data with the EAX protocol
   @param eax     The EAX state
   @param ct      The ciphertext
   @param pt      [out] The plaintext
   @param length  The length (octets) of the ciphertext
   @return CRYPT_OK if successful
*/
int eax_decrypt(eax_state *eax, const unsigned char *ct, unsigned char *pt,
                unsigned long length)
{
   int err;

   LTC_ARGCHK(eax != NULL);
   LTC_ARGCHK(pt  != NULL);
   LTC_ARGCHK(ct  != NULL);

   /* omac ciphertext */
   if ((err = omac_process(&eax->ctomac, ct, length)) != CRYPT_OK) {
      return err;
   }

   /* decrypt  */
   return ctr_decrypt(ct, pt, length, &eax->ctr);
}

#endif

/* ref:         HEAD -> master, tag: v1.18.2 */
/* git commit:  7e7eb695d581782f04b24dc444cbfde86af59853 */
/* commit time: 2018-07-01 22:49:01 +0200 */
