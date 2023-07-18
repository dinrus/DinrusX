#include <Core/Core.h>
#include <DinrusX/CoreX/BaseTypes.h>
#include <DinrusX/DinrusXSys/ZLibCompressor.h>

void MD5Init (SMD5Context *context);
void MD5Update (SMD5Context *context, const unsigned char *input, uint32 inputLen);
void MD5Final (unsigned char *digest, SMD5Context *context);
