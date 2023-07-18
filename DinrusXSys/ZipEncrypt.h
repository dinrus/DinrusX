// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef _ZIP_ENCRYPT_HDR_
#define _ZIP_ENCRYPT_HDR_
/*
#ifdef SUPPORT_RSA_AND_STREAMCIPHER_PAK_ENCRYPTION
#undef SUPPORT_RSA_AND_STREAMCIPHER_PAK_ENCRYPTION
#endif

#ifdef INCLUDE_LIBTOMCRYPT
#undef INCLUDE_LIBTOMCRYPT
#endif

#define SUPPORT_UNSIGNED_PAKS
//Временно аннулируем использование шифрованных зип-архивов,
//так как требуется tomcrypt...

//Далее код не должен сработать!
*/
//#define SUPPORT_RSA_AND_STREAMCIPHER_PAK_ENCRYPTION 1
//#define INCLUDE_LIBTOMCRYPT 1

//#ifdef SUPPORT_RSA_AND_STREAMCIPHER_PAK_ENCRYPTION

#include <DinrusX/DinrusXSys/ZipFileFormat.h>
#include <DinrusX/DinrusXSys/ZipDirStructures.h>

//#ifdef INCLUDE_LIBTOMCRYPT
	#include <DinrusX/3rdParty/tomcrypt/tomcrypt.h>
	#include <DinrusX/CoreX/Assert/DrxAssert.h> // tomcrypt will re-define assert
	#undef byte                           // tomcrypt defines a byte macro which conflicts with out byte data type
	#define STREAM_CIPHER_NAME "twofish"
extern prng_state g_yarrow_prng_state;
extern rsa_key g_rsa_key_public_for_sign;
//#endif //INCLUDE_LIBTOMCRYPT

namespace ZipEncrypt
{
//#ifdef INCLUDE_LIBTOMCRYPT
void Init(const uint8* pKeyData, uint32 keyLen);
bool StartStreamCipher(unsigned char key[16], unsigned char IV[16], symmetric_CTR* pCTR, const unsigned int offset = 0);
void FinishStreamCipher(symmetric_CTR* pCTR);
bool DecryptBufferWithStreamCipher(unsigned char* inBuffer, unsigned char* outBuffer, size_t bufferSize, symmetric_CTR* pCTR);
bool DecryptBufferWithStreamCipher(unsigned char* inBuffer, size_t bufferSize, unsigned char key[16], unsigned char IV[16]);
int  GetEncryptionKeyIndex(const ZipDir::FileEntry* pFileEntry);
void GetEncryptionInitialVector(const ZipDir::FileEntry* pFileEntry, unsigned char IV[16]);

bool RSA_VerifyData(void* inBuffer, int sizeIn, unsigned char* signedHash, int signedHashSize, rsa_key& publicKey);
bool RSA_VerifyData(const unsigned char** inBuffers, unsigned int* sizesIn, const int numBuffers, unsigned char* signedHash, int signedHashSize, rsa_key& publicKey);

int  custom_rsa_encrypt_key_ex(const unsigned char* in, unsigned long inlen,
                               unsigned char* out, unsigned long* outlen,
                               const unsigned char* lparam, unsigned long lparamlen,
                               prng_state* prng, int prng_idx, int hash_idx, int padding, rsa_key* key);

int custom_rsa_decrypt_key_ex(const unsigned char* in, unsigned long inlen,
                              unsigned char* out, unsigned long* outlen,
                              const unsigned char* lparam, unsigned long lparamlen,
                              int hash_idx, int padding,
                              int* stat, rsa_key* key);
//#endif  //INCLUDE_LIBTOMCRYPT
};
//#endif
#endif //_ZIP_ENCRYPT_HDR_
