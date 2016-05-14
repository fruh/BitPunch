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

    if (BPU_gf2VecMalloc(&pt_in, ctx->pt_len)) {
        BPU_printError("PT initialisation error");
        goto err;
    }

    BPU_gf2VecRand(pt_in, 0);

    if (BPU_gf2VecMalloc(&ct, ctx->ct_len)) {
        BPU_printError("CT vector allocation error");
        BPU_gf2VecFree(&pt_in);
        return 1;
    }

    if (BPU_gf2VecMalloc(&pt_out, ctx->pt_len)) {
        BPU_printError("PT out initialisation error");
        return 1;
    }

    BPU_gf2VecRand(pt_out, 0);

    fprintf(stderr, "Encryption...\n");
    if (BPU_mecsEncrypt(ct, pt_in, ctx, NULL)) {
        BPU_printError("Encryption error");

        BPU_gf2VecFree(&ct);
        BPU_gf2VecFree(&pt_in);
        BPU_gf2VecFree(&pt_out);
        return 1;
    }

    fprintf(stderr, "Decryption...\n");
    if (BPU_mecsDecrypt(pt_out, error, ct, ctx)) {
        BPU_printError("Decryption error");

        BPU_gf2VecFree(&ct);
        BPU_gf2VecFree(&pt_in);
        BPU_gf2VecFree(&pt_out);
        return 1;
    }

    if (BPU_gf2VecCmp(pt_in, pt_out)) {
        BPU_printError("\nOutput plain text differs from input");

        rc = 2;
    }
    else {
        fprintf(stderr,
                "\nSUCCESS: Input plain text is equal to output plain text.\n");
    }

err:
    fprintf(stderr, "\nCleaning up...\n");
    BPU_gf2VecFree(&pt_in);
    BPU_gf2VecFree(&pt_out);
    BPU_gf2VecFree(&ct);
    return rc;
}

int main(int argc, char **argv) {
    int rc = BPU_ERROR;
    BPU_T_Mecs_Ctx *ctx = NULL; // MUST BE NULL
    BPU_T_UN_Mecs_Params params;

    srand(time(NULL));

    fprintf(stderr, "Basic GOPPA Initialisation...\n");
    if (BPU_mecsInitParamsGoppa(&params, 11, 50, 0)) {
        goto err;
    }

    if (BPU_mecsInitCtx(&ctx, &params, BPU_EN_MECS_BASIC_GOPPA)) {
        goto err;
    }

    rc += testKeyGenEncDec(ctx);

    rc = BPU_SUCCESS;
err:
    BPU_SAFE_FREE(BPU_mecsFreeCtx, ctx);
    return rc;
}
