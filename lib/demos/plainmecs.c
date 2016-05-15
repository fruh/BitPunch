/*
 This file is part of BitPunch
 Copyright (C) 2016 Marek Klein <kleinmrk[what here]gmail.com>

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

int testKeyGenEncDec(BPU_T_Mecs_Ctx * ctx) {
    BPU_T_GF2_Vector *ct = NULL;
    BPU_T_GF2_Vector *pt_in = NULL;
    BPU_T_GF2_Vector *pt_out = NULL;
    BPU_T_GF2_Vector *error = NULL;
    int rc = BPU_ERROR;

    fprintf(stderr, "Key generation...\n");
    if (BPU_mecsGenKeyPair(ctx)) {
        BPU_printError("Key generation error");
        goto err;
    }

    pt_in = BPU_gf2VecNew(ctx->pt_len);
    if (NULL == pt_in) {
        BPU_printError("BPU_gf2VecNew failed");
        goto err;
    }

    if (BPU_SUCCESS != BPU_gf2VecRand(pt_in, 0)) {
        BPU_printError("BPU_gf2VecRand failed");
        goto err;
    }

    ct = BPU_gf2VecNew(ctx->ct_len);
    if (NULL == ct) {
        BPU_printError("BPU_gf2VecNew failed");
        goto err;
    }

    pt_out = BPU_gf2VecNew(ctx->pt_len);
    if (NULL == pt_out) {
        BPU_printError("BPU_gf2VecNew failed");
        goto err;
    }

    BPU_gf2VecRand(pt_out, 0);

    fprintf(stderr, "Encryption...\n");
    if (BPU_mecsEncrypt(ct, pt_in, ctx, NULL)) {
        BPU_printError("BPU_mecsEncrypt failed");
        goto err;
    }

    fprintf(stderr, "Decryption...\n");

    error = BPU_gf2VecNew(ct->len);
    if (NULL == error) {
        BPU_printError("BPU_gf2VecNew failed");
        goto err;
    }

    if (BPU_mecsDecrypt(pt_out, error, ct, ctx)) {
        BPU_printError("Decryption error");
        goto err;
    }

    if (BPU_gf2VecCmp(pt_in, pt_out)) {
        BPU_printError("\nOutput plain text differs from input");
        goto err;
    }
    else {
        fprintf(stderr,
                "\nSUCCESS: Input plain text is equal to output plain text.\n");
    }

    rc = BPU_SUCCESS;
err:
    fprintf(stderr, "\nCleaning up...\n");
    BPU_gf2VecFree(pt_in);
    BPU_gf2VecFree(pt_out);
    BPU_gf2VecFree(ct);
    BPU_gf2VecFree(error);
    return rc;
}

int main(int argc, char **argv) {
    int rc = BPU_ERROR;
    BPU_T_Mecs_Ctx *ctx = NULL; // MUST BE NULL
    BPU_T_UN_Mecs_Params *params = NULL;

    srand(time(NULL));

    fprintf(stderr, "Basic GOPPA Initialisation...\n");
    params = BPU_mecsParamsGoppaNew(11, 50, 0);
    if (NULL == params) {
        BPU_printError("BPU_mecsParamsGoppaNew failed");
        goto err;
    }

    ctx = BPU_mecsCtxNew(params, BPU_EN_MECS_BASIC_GOPPA);
    if (NULL == ctx) {
        BPU_printError("BPU_mecsCtxNew failed");
        goto err;
    }

    if (BPU_SUCCESS != testKeyGenEncDec(ctx)) {
        BPU_printError("testKeyGenEncDec failed");
        goto err;
    }

    rc = BPU_SUCCESS;
err:
    BPU_SAFE_FREE(BPU_mecsFreeCtx, ctx);
    BPU_SAFE_FREE(BPU_mecsDestroyParamsGoppa, params);
    return rc;
}
