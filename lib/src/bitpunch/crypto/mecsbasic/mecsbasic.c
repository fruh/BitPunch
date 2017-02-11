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
#include <bitpunch/bitpunch.h>
#include <bitpunch/crypto/mecsbasic/mecsbasic.h>

#include <bitpunch/math/gf2.h>
#include <bitpunch/debugio.h>

#ifdef BPU_CONF_ENCRYPTION
int BPU_mecsBasicEncrypt(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in,
                         const BPU_T_Mecs_Ctx * ctx, BPU_T_GF2_Vector * error)
{
    int rc = BPU_ERROR;
    BPU_T_GF2_Vector *local_error = NULL;

    if (NULL == out) {
        BPU_printError("Invalid input parameter \"%s\"", "out");
        goto err;
    }

    if (NULL == in) {
        BPU_printError("Invalid input parameter \"%s\"", "in");
        goto err;
    }

    if (NULL == ctx) {
        BPU_printError("Invalid input parameter \"%s\"", "ctx");
        goto err;
    }

    BPU_gf2VecNull(out);

    if (BPU_SUCCESS != ctx->code_ctx->_encode(out, in, ctx->code_ctx)) {
        BPU_printError("encode failed");
        goto err;
    }

    if (NULL != error) {
        local_error = error;
    } else {
        // generate random error vector e if needed
        local_error = BPU_gf2VecNew(ctx->code_ctx->code_len);
        if (BPU_SUCCESS != BPU_gf2VecRand(local_error, ctx->code_ctx->t)) {
            BPU_printError("BPU_gf2VecRand failed");
            goto err;
        }
    }

    // z' XOR e
    if (BPU_SUCCESS != BPU_gf2VecXor(out, local_error)) {
        BPU_printError("BPU_gf2VecXor failed");
        goto err;
    }

    rc = BPU_SUCCESS;
 err:
    if (NULL == error) {
        BPU_SAFE_FREE(BPU_gf2VecFree, local_error);
    }

    return rc;
}
#endif

#ifdef BPU_CONF_DECRYPTION
int BPU_mecsBasicDecrypt(BPU_T_GF2_Vector * out,
                         BPU_T_GF2_Vector * error,
                         const BPU_T_GF2_Vector * in,
                         const BPU_T_Mecs_Ctx * ctx)
{
    int rc = BPU_ERROR;
    BPU_T_GF2_Vector *temp = NULL;

    if (NULL == (temp = BPU_gf2VecNew(in->len))) {
        BPU_printError("BPU_gf2VecNew failed");
        goto err;
    }

    if (BPU_SUCCESS != BPU_gf2VecCopy(temp, in)) {
        BPU_printError("BPU_gf2VecCopy failed");
        goto err;
    }

    if (BPU_SUCCESS != ctx->code_ctx->_decode(out, error, temp, ctx->code_ctx)) {
        BPU_printError("_decode failed");
        goto err;
    }

    rc = BPU_SUCCESS;
err:
    BPU_SAFE_FREE(BPU_gf2VecFree, temp);
    return rc;
}
#endif
