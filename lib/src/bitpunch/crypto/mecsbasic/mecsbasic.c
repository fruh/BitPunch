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
#include <bitpunch/crypto/mecsbasic/mecsbasic.h>

#include <bitpunch/math/gf2.h>
#include <bitpunch/debugio.h>

#ifdef BPU_CONF_ENCRYPTION
int BPU_mecsBasicEncrypt(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in,
                         const BPU_T_Mecs_Ctx * ctx, BPU_T_GF2_Vector * error) {
    int rc;
    BPU_T_GF2_Vector *local_error = NULL;

    if (NULL != error) {
        local_error = error;
    }
    else {
        BPU_gf2VecMalloc(&local_error, ctx->code_ctx->code_len);
    }
    BPU_gf2VecNull(out);

    // test the size of message and g_m
    if (in->len != ctx->code_ctx->msg_len) {
        BPU_printError("message length has to be of length %d",
                       ctx->code_ctx->msg_len);

        return -1;
    }

    rc = ctx->code_ctx->_encode(out, in, ctx->code_ctx);
    if (rc) {
        BPU_printError("can not encode");
        return rc;
    }

    // generate random error vector e
    rc += BPU_gf2VecRand(local_error, ctx->code_ctx->t);

    if (rc) {
        BPU_printError("can not init rand vector");
        return rc;
    }

    // z' XOR e
    rc = BPU_gf2VecXor(out, local_error);

    if (rc) {
        BPU_printError("can not add error vector");
        return rc;
    }
    return rc;
}
#endif

#ifdef BPU_CONF_DECRYPTION
int BPU_mecsBasicDecrypt(BPU_T_GF2_Vector * out,
                         BPU_T_GF2_Vector * error,
                         const BPU_T_GF2_Vector * in,
                         const BPU_T_Mecs_Ctx * ctx) {
    int rc = 0;
    BPU_T_GF2_Vector *temp;

    BPU_gf2VecMalloc(&temp, in->len);
    BPU_gf2VecCopy(temp, in);

    BPU_gf2VecMalloc(&error, in->len);

    rc = ctx->code_ctx->_decode(out, error, temp, ctx->code_ctx);

    BPU_gf2VecFree(&temp);

    return rc;
}
#endif
