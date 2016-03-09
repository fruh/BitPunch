/*
This file is part of BitPunch
Copyright (C) 2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "aes.h"

#ifdef BPU_CONF_AES
#include "mbedtls/mbedtls/aes.h"

#include <bitpunch/debugio.h>
#include <bitpunch/math/gf2.h>

/*int BPU_gf2VecHash(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in) {
	uint8_t md[BPU_HASH_LEN];

	if (out->len != BPU_HASH_LEN * 8) {
		BPU_printError("Wrong vector len %d", out->len);

		return -1;
	}
	// if input len is not divisible by 4, then it is not complete vector
	if (in->len % 8) {
//		BPU_printError("Wrong input vector len %d, should be divisible by 8", in->len);
		BPU_printWarning("input vector len %d, should be divisible by 8", in->len);
	}
	// hash vector
	sha512((uint8_t *)in->elements, in->len / 8, md, 0);
	// copy digest to vector
	memcpy(out->elements, md, BPU_HASH_LEN);

	return 0;
}*/

int BPU_gf2VecAesEnc(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in,  BPU_T_GF2_Vector *key,  BPU_T_GF2_Vector *iv){
    uint8_t output[out->len/8];
    if (in->len % 16) {
            BPU_printWarning("input vector len %d, should be divisible by 16", in->len);
    }
    mbedtls_aes_context enc_ctx;

    BPU_printGf2Vec(in);

    mbedtls_aes_init(&enc_ctx);
    mbedtls_aes_setkey_enc(&enc_ctx, (uint8_t *) key->elements, 256);
    //mbedtls_aes_crypt_cbc(&enc_ctx, MBEDTLS_AES_ENCRYPT, in->len / 8, (uint8_t *) iv->elements, (uint8_t *) in->elements, output );
    mbedtls_aes_crypt_cbc(&enc_ctx, MBEDTLS_AES_ENCRYPT, in->len / (BITS_PER_BYTE), (uint8_t *) iv->elements,(uint8_t *) in->elements, output);
    memcpy(out->elements, output, out->len / 8);
    //BPU_printGf2Vec(out);
    //mbedtls_aes_free( &enc_ctx );
    return 0;
}

int BPU_gf2VecAesDec(BPU_T_GF2_Vector *out,  const BPU_T_GF2_Vector *in, BPU_T_GF2_Vector *key,  BPU_T_GF2_Vector *iv){
    uint8_t output[out->len/8];
    if (in->len % 16) {
            BPU_printWarning("input vector len %d, should be divisible by 16", in->len);
    }
    mbedtls_aes_context dec_ctx;
    //BPU_printGf2Vec(in);
    mbedtls_aes_init(&dec_ctx);
    mbedtls_aes_setkey_dec(&dec_ctx, (uint8_t *) key->elements, 256);
    mbedtls_aes_crypt_cbc(&dec_ctx, MBEDTLS_AES_DECRYPT, in->len / (BITS_PER_BYTE), (uint8_t *) iv->elements, (uint8_t *) in->elements, output );
    memcpy(out->elements, output, out->len / 8);
    BPU_printGf2Vec(out);
    //mbedtls_aes_free( &dec_ctx );
    return 0;
}


#endif
