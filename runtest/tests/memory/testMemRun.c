/*
 This file is part of BitPunch
 Copyright (C) 2014-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

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
#include <bitpunch/bitpunch.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include <bitpunch/crypto/hash/sha512.h>
#include <bitpunch/asn1/asn1.h>

int testKeyGenEncDec(BPU_T_Mecs_Ctx* ctx)
{
    BPU_T_GF2_Vector *ct = NULL;
    BPU_T_GF2_Vector *pt_in = NULL;
    BPU_T_GF2_Vector *pt_out= NULL;
    BPU_T_GF2_Vector *error = NULL;
    int rc = 0;

    fprintf(stderr, "Key generation...\n");
    if (BPU_mecsGenKeyPair(ctx)) {
        BPU_printError("Key generation error");
        return 1;
    }

    // prepare plain text, allocate memory and init random plaintext
    if (NULL == (pt_in = BPU_gf2VecNew(ctx->pt_len))) {
        BPU_printError("PT initialisation error");
        return 1;
    }

    BPU_gf2VecRand(pt_in, 0);

    // alocate cipher text vector
    if (NULL == (ct = BPU_gf2VecNew(ctx->ct_len))) {
        BPU_printError("CT vector allocation error");
        BPU_gf2VecFree(pt_in);
        return 1;
    }

    // prepare plain text, allocate memory and init random plaintext
    if (NULL == (pt_out = BPU_gf2VecNew(ctx->pt_len))) {
        BPU_printError("PT out initialisation error");
        return 1;
    }

    BPU_gf2VecRand(pt_out, 0);

    fprintf(stderr, "Encryption...\n");
    if (BPU_mecsEncrypt(ct, pt_in, ctx, NULL)) {
        BPU_printError("Encryption error");

        BPU_gf2VecFree(ct);
        BPU_gf2VecFree(pt_in);
        BPU_gf2VecFree(pt_out);
        return 1;
    }


    // alocate cipher text vector
    if (NULL == (error = BPU_gf2VecNew(ctx->ct_len))) {
        BPU_printError("BPU_gf2VecNew failed");
        BPU_gf2VecFree(pt_in);
        return 1;
    }

    fprintf(stderr, "Decryption...\n");
    if (BPU_mecsDecrypt(pt_out, error, ct, ctx)) {
        BPU_printError("Decryption error");

        BPU_gf2VecFree(ct);
        BPU_gf2VecFree(pt_in);
        BPU_gf2VecFree(pt_out);
        return 1;
    }

    // check for correct decryption
    if (BPU_gf2VecCmp(pt_in, pt_out)) {
        BPU_printError("\nOutput plain text differs from input");
        rc = 2;
    } else {
        fprintf(stderr,
                "\nSUCCESS: Input plain text is equal to output plain text.\n");
    }

    fprintf(stderr, "\nCleaning up...\n");
    BPU_gf2VecFree(pt_in);
    BPU_gf2VecFree(pt_out);
    BPU_gf2VecFree(ct);
    return rc;
}

#ifdef BPU_CONF_ASN1
int testKeyGenAsn1()
{
    int rc = 0;
    BPU_T_Mecs_Ctx *ctx = NULL;
    BPU_T_UN_Mecs_Params *params = NULL;

    /***************************************/
    // mce initialisation t = 50, m = 11
    fprintf(stderr, "Basic GOPPA Initialisation...\n");
    if (NULL == (params = BPU_mecsParamsGoppaNew(11, 50, 0))) {
        return 1;
    }
    if (NULL == (ctx = BPU_mecsCtxNew(params, BPU_EN_MECS_BASIC_GOPPA))) {
        return 1;
    }

    fprintf(stderr, "Key generation...\n");
    if (BPU_mecsGenKeyPair(ctx)) {
        BPU_printError("Key generation error");
        return 1;
    }

    rc = BPU_asn1SaveKeyPair(ctx, "prikey.der", "pubkey.der");
    if (rc) {
        asn1_perror(rc);
    }

    BPU_mecsFreeCtx(ctx);
    rc = BPU_asn1LoadKeyPair(&ctx, "prikey.der", "pubkey.der");
    if (rc) {
        asn1_perror(rc);
    }

    BPU_mecsFreeCtx(ctx);
    BPU_mecsDestroyParamsGoppa(params);
    return rc;
}
#endif

int main(int argc, char **argv)
{
    int rc = 0;
    BPU_T_Mecs_Ctx *ctx = NULL;
    BPU_T_UN_Mecs_Params *params = NULL;

    srand(time(NULL));
#if defined(BPU_CONF_GOPPA_WITH_H) && defined(BPU_CONF_ASN1)
    testKeyGenAsn1();
#endif
    if (NULL == (params = BPU_mecsParamsGoppaNew(11, 50, 0))) {
        return 1;
    }

    fprintf(stderr, "Basic GOPPA Initialisation...\n");
    if (NULL == (ctx = BPU_mecsCtxNew(params, BPU_EN_MECS_BASIC_GOPPA))) {
        return 1;
    }

    rc += testKeyGenEncDec(ctx);
    BPU_mecsFreeCtx(ctx);

#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
    fprintf(stderr, "\nCCA2 Pointcheval GOPPA Initialisation...\n");
    if (NULL == (ctx = BPU_mecsCtxNew(11, 50, BPU_EN_MECS_CCA2_POINTCHEVAL_GOPPA))) {
        return 1;
    }

    rc += testKeyGenEncDec(ctx);
    BPU_mecsFreeCtx(ctx);
    BPU_mecsDestroyParamsGoppa(params);
#endif
    return rc;
}
