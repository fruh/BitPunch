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
#include "mecspointcheval.h"

#if defined(BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA) || defined(BPU_CONF_MECS_CCA2_POINTCHEVAL_QCMDPC)
#include <bitpunch/debugio.h>
#include <bitpunch/math/gf2.h>
#include <bitpunch/crypto/mecsbasic/mecsbasic.h>

#ifdef BPU_CONF_ENCRYPTION
int BPU_mecsPointchevalCCA2Encrypt(BPU_T_GF2_Vector * out,
                                   const BPU_T_GF2_Vector * in,
                                   const BPU_T_Mecs_Ctx * ctx) {
    BPU_T_GF2_Vector *r1, *r2, *cca2_pt, *hash, *hash_in, *enc_pt, *tmp;
    int rc = 0;

    // Generate a random (k − l)-bit vector r1 and a random l-bit vector r2
    BPU_gf2VecMalloc(&r2, ctx->pt_len);
    BPU_gf2VecMalloc(&r1, ctx->code_ctx->msg_len - ctx->pt_len);
    BPU_gf2VecRand(r1, 0);
    BPU_gf2VecRand(r2, 0);

    BPU_gf2VecMalloc(&hash, ctx->pt_len);
    BPU_gf2VecMalloc(&hash_in, 2 * ctx->pt_len);
    // Create CCA2-safe plaintext m= r1 ∥ hash (m ∥ r2 )
    BPU_gf2VecConcat(hash_in, in, r2);

    BPU_gf2VecHash(hash, hash_in);
    BPU_gf2VecFree(&hash_in);

    BPU_gf2VecMalloc(&cca2_pt, r1->len + hash->len);
    BPU_gf2VecConcat(cca2_pt, r1, hash);

    BPU_gf2VecMalloc(&enc_pt, ctx->code_ctx->code_len);

    // encrypt with basic MECS
    if (BPU_mecsBasicEncrypt(enc_pt, cca2_pt, ctx)) {
        return -1;
    }
    BPU_gf2VecFree(&cca2_pt);

    // add CCA2-safe data extension z = (z′ ⊕ e) ∥ (hash (r1) ⊕ m) ∥ (hash (e) ⊕ r2 )
    BPU_gf2VecMalloc(&tmp, enc_pt->len + hash->len);
    BPU_gf2VecHash(hash, r1);
    BPU_gf2VecFree(&r1);
    BPU_gf2VecXor(hash, in);
    BPU_gf2VecConcat(tmp, enc_pt, hash);
    BPU_gf2VecFree(&enc_pt);

    BPU_gf2VecHash(hash, ctx->code_ctx->e);
    BPU_gf2VecXor(hash, r2);
    BPU_gf2VecFree(&r2);
    BPU_gf2VecConcat(out, tmp, hash);

    BPU_gf2VecFree(&hash);
    BPU_gf2VecFree(&tmp);

    return rc;
}
#endif // BPU_CONF_ENCRYPTION

#ifdef BPU_CONF_DECRYPTION
int BPU_mecsPointchevalCCA2Decrypt(BPU_T_GF2_Vector * out,
                                   const BPU_T_GF2_Vector * in,
                                   const BPU_T_Mecs_Ctx * ctx) {
    BPU_T_GF2_Vector *z1, *z3;  // n, l, l-bit
    BPU_T_GF2_Vector *tmp_2, *pt_cca2;
    BPU_T_GF2_Vector *r;        // k - l
    BPU_T_GF2_Vector *h, *h_tmp;
    int rc = 0;

    // split ct in z1, z2, z3
    BPU_gf2VecMalloc(&z1, ctx->code_ctx->code_len);
    // z2 is like out
    BPU_gf2VecMalloc(&z3, ctx->pt_len);

    // Split z to ( z1 , z2 , z3 )
    BPU_gf2VecCrop(z1, in, 0, z1->len);
    BPU_gf2VecCrop(out, in, z1->len, out->len);
    BPU_gf2VecCrop(z3, in, z1->len + out->len, z3->len);

    BPU_gf2VecMalloc(&pt_cca2, ctx->code_ctx->msg_len);
    // decrypt z1 using basic mecs  Reconstruct the CCA2-safe plaintext m′ = z1 ⊕ e
    if (BPU_mecsBasicDecrypt(pt_cca2, z1, ctx)) {
        return -1;
    }
    BPU_gf2VecFree(&z1);

    BPU_gf2VecMalloc(&r, ctx->code_ctx->msg_len - ctx->pt_len);
    BPU_gf2VecMalloc(&h, ctx->pt_len);
    BPU_gf2VecCrop(r, pt_cca2, 0, r->len);
    BPU_gf2VecCrop(h, pt_cca2, r->len, h->len);
    BPU_gf2VecFree(&pt_cca2);

    BPU_gf2VecMalloc(&h_tmp, ctx->pt_len);
    // Reconstruct plaintext candidate m = z2 ⊕ hash (r)
    BPU_gf2VecHash(h_tmp, r);
    BPU_gf2VecFree(&r);
    BPU_gf2VecXor(out, h_tmp);

    // Determine check value h′ = hash (m ∥ hash (e) ⊕ z3 ).
    BPU_gf2VecHash(h_tmp, ctx->code_ctx->e);
    BPU_gf2VecXor(h_tmp, z3);
    BPU_gf2VecFree(&z3);

    BPU_gf2VecMalloc(&tmp_2, ctx->pt_len * 2);
    BPU_gf2VecConcat(tmp_2, out, h_tmp);
    BPU_gf2VecHash(h_tmp, tmp_2);
    BPU_gf2VecFree(&tmp_2);

    if (BPU_gf2VecCmp(h, h_tmp)) {
        BPU_printError("Wrong check value.");

        rc = -1;
    }
    BPU_gf2VecFree(&h);
    BPU_gf2VecFree(&h_tmp);
    return rc;
}
#endif // BPU_CONF_DECRYPTION

#endif // BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
