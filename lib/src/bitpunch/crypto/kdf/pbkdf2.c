#include "pbkdf2.h"

#ifdef BPU_CONF_KDF
#include "mbedtls/mbedtls/pkcs5.h"

#include <bitpunch/debugio.h>
#include <bitpunch/math/gf2.h>

int BPU_gf2VecKDF(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in) {
    int err;
    mbedtls_md_context_t ctx;
    unsigned char output[64];
    memset(output,0,64);
    unsigned char pwd[48];
    memset(pwd,0,48);
    int plen = 48;
    unsigned char salt[48];
    memset(salt,0,48);
    int slen = 48;
    mbedtls_md_init( &ctx );
    const mbedtls_md_info_t *info_sha512;
    info_sha512 = mbedtls_md_info_from_type( MBEDTLS_MD_SHA512 );
    if( info_sha512 == NULL ) {
        fprintf(stderr,"\nNENASIEL SOM HASH1\n");
        return -1;
    }
    if( ( err = mbedtls_md_setup( &ctx, info_sha512, 1 ) ) != 0 )
    {
        fprintf(stderr,"\nNENASIEL SOM HASH2\n");
        return -1;
     }
    err = mbedtls_pkcs5_pbkdf2_hmac( &ctx, (unsigned char*)pwd, plen,(unsigned char*)salt, slen, 1000, (unsigned) 48, (unsigned char*)output );
    fprintf(stderr,"\nCHYBA JE: %d\n",err);
    memcpy(out->elements, output, out->len / 8);
    fprintf(stderr,"\nTLACIM KDF:\n");
    BPU_printGf2Vec(out);
    mbedtls_md_free( &ctx );
	return 0;
}
#endif
