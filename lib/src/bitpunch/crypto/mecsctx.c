/*
This file is part of BitPunch
Copyright (C) 2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
Copyright (C) 2015 Andrej Gulyas <andrej.guly[what here]gmail.com>

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
#include "mecsctx.h"

#include <stdlib.h>

#include <bitpunch/debugio.h>
#include <bitpunch/errorcodes.h>
#include <bitpunch/code/codectx.h>
#include <bitpunch/math/gf2x.h>

// additional encryption schemes
#include <bitpunch/crypto/mecsbasic/mecsbasic.h>
#include <bitpunch/code/goppa/goppa.h>
#include <bitpunch/code/qcmdpc/qcmdpc.h>

#if defined(BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA) || defined(BPU_CONF_MECS_CCA2_POINTCHEVAL_QCMDPC)
#include <bitpunch/crypto/cca2/mecspointcheval.h>
#endif

int BPU_mecsInitCtx(BPU_T_Mecs_Ctx ** ctx, const BPU_T_UN_Mecs_Params * params,
                    const BPU_T_EN_Mecs_Types type) {
    int rc = 0;
    BPU_T_Mecs_Ctx *ctx_p;

    if (!*ctx) {
        *ctx = (BPU_T_Mecs_Ctx *) calloc(sizeof(BPU_T_Mecs_Ctx), 1);

        if (!*ctx) {
            BPU_printError("can not alloc Mecs ctx");
            return -1;
        }
    }
    else {
        BPU_printDebug("Already initialized");
        return 0;
    }
    ctx_p = *ctx;
    ctx_p->type = type;

    switch (type) {
    case BPU_EN_MECS_BASIC_GOPPA:
#ifdef BPU_CONF_ENCRYPTION
        ctx_p->_encrypt = BPU_mecsBasicEncrypt;
#endif
#ifdef BPU_CONF_DECRYPTION
        ctx_p->_decrypt = BPU_mecsBasicDecrypt;
#endif
#ifdef BPU_CONF_KEY_GEN
        ctx_p->_genKeyPair = BPU_goppaGenCode;
#endif
        rc = BPU_codeInitCtx(&ctx_p->code_ctx, (BPU_T_UN_Code_Params *) params,
                             BPU_EN_CODE_GOPPA);
        if (rc) {
            return rc;
        }
        ctx_p->pt_len = ctx_p->code_ctx->msg_len;
        ctx_p->ct_len = ctx_p->code_ctx->code_len;
        break;

#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
    case BPU_EN_MECS_CCA2_POINTCHEVAL_GOPPA:
#ifdef BPU_CONF_ENCRYPTION
        ctx_p->_encrypt = BPU_mecsPointchevalCCA2Encrypt;
#endif
#ifdef BPU_CONF_DECRYPTION
        ctx_p->_decrypt = BPU_mecsPointchevalCCA2Decrypt;
#endif
#ifdef BPU_CONF_KEY_GEN
        ctx_p->_genKeyPair = BPU_goppaGenCode;
#endif
        rc = BPU_codeInitCtx(&ctx_p->code_ctx, (BPU_T_UN_Code_Params *) params,
                             BPU_EN_CODE_GOPPA);
        if (rc) {
            return rc;
        }
        ctx_p->pt_len =
            BPU_HASH_LEN * 8 <
            ctx_p->code_ctx->msg_len ? BPU_HASH_LEN *
            8 : ctx_p->code_ctx->msg_len;
        ctx_p->ct_len = ctx_p->code_ctx->code_len + 2 * ctx_p->ct_len;
        break;
#endif

    case BPU_EN_MECS_BASIC_QCMDPC:
#ifdef BPU_CONF_ENCRYPTION
        ctx_p->_encrypt = BPU_mecsBasicEncrypt;
#endif
#ifdef BPU_CONF_DECRYPTION
        ctx_p->_decrypt = BPU_mecsBasicDecrypt;
#endif
#ifdef BPU_CONF_KEY_GEN
        ctx_p->_genKeyPair = BPU_mecsQcmdpcGenKeys;
#endif
        rc = BPU_codeInitCtx(&ctx_p->code_ctx, (BPU_T_UN_Code_Params *) params,
                             BPU_EN_CODE_QCMDPC);
        if (rc) {
            return rc;
        }
        ctx_p->pt_len = ctx_p->code_ctx->msg_len;
        ctx_p->ct_len = ctx_p->code_ctx->code_len;
        break;

#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_QCMDPC
    case BPU_EN_MECS_CCA2_POINTCHEVAL_QCMDPC:
#ifdef BPU_CONF_ENCRYPTION
        ctx_p->_encrypt = BPU_mecsPointchevalCCA2Encrypt;
#endif
#ifdef BPU_CONF_DECRYPTION
        ctx_p->_decrypt = BPU_mecsPointchevalCCA2Decrypt;
#endif
#ifdef BPU_CONF_KEY_GEN
        ctx_p->_genKeyPair = BPU_mecsQcmdpcGenKeys;
#endif
        rc = BPU_codeInitCtx(&ctx_p->code_ctx, (BPU_T_UN_Code_Params *) params,
                             BPU_EN_CODE_QCMDPC);
        if (rc) {
            return rc;
        }
        ctx_p->pt_len =
            BPU_HASH_LEN * 8 <
            ctx_p->code_ctx->msg_len ? BPU_HASH_LEN *
            8 : ctx_p->code_ctx->msg_len;
        ctx_p->ct_len = ctx_p->code_ctx->code_len + 2 * ctx_p->ct_len;
        break;
#endif
        /* EXAMPLE please DO NOT REMOVE
           case BPU_EN_MECS_*****:
           #ifdef BPU_CONF_ENCRYPTION
           ctx_p->_encrypt = FUNC_FROM_YOUR_FILE;
           #endif
           #ifdef BPU_CONF_DECRYPTION
           ctx_p->_decrypt = FUNC_FROM_YOUR_FILE;
           #endif
           #ifdef BPU_CONF_KEY_GEN
           ctx_p->_genKeyPair = FUNC_FROM_YOUR_FILE;
           #endif
           rc = BPU_codeInitCtx(&ctx_p->code_ctx, m, t, BPU_EN_CODE_GOPPA, mod);
           if (rc) {
           return rc;
           }
           ctx->pt_len = PT_LEN;
           ctx->ct_len = CT_LEN;
           break;
         */
    default:
        BPU_printError("MECS type not supported: %d", type);
        return BPU_EC_MECS_TYPE_NOT_SUPPORTED;
    }
    return rc;
}

int BPU_mecsFreeCtx(BPU_T_Mecs_Ctx ** ctx) {
    BPU_T_Mecs_Ctx *ctx_p = *ctx;

    if (!ctx_p) {
        return 0;
    }
    switch (ctx_p->type) {
    case BPU_EN_MECS_BASIC_GOPPA:
#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
    case BPU_EN_MECS_CCA2_POINTCHEVAL_GOPPA:
#endif
        ctx_p->_encrypt = NULL;
        ctx_p->_decrypt = NULL;
        ctx_p->_genKeyPair = NULL;
        break;
    case BPU_EN_MECS_BASIC_QCMDPC:
#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_QCMDPC
    case BPU_EN_MECS_CCA2_POINTCHEVAL_QCMDPC:
#endif
        ctx_p->_encrypt = NULL;
        ctx_p->_decrypt = NULL;
        ctx_p->_genKeyPair = NULL;
        break;
    default:
        BPU_printError("MECS type not supported: %d", ctx_p->type);
        return BPU_EC_MECS_TYPE_NOT_SUPPORTED;
    }
    BPU_codeFreeCtx(&ctx_p->code_ctx);
    free(ctx_p);
    *ctx = NULL;

    return 0;
}

int BPU_mecsInitParamsGoppa(BPU_T_UN_Mecs_Params * params, const uint16_t m,
                            const uint16_t t, const BPU_T_GF2_16x mod) {
    return BPU_codeInitParamsGoppa((BPU_T_UN_Code_Params *) params, m, t, mod);
}

void BPU_mecsFreeParamsGoppa(BPU_T_UN_Mecs_Params * params) {
    BPU_codeFreeParamsGoppa((BPU_T_UN_Code_Params *) params);
}

int BPU_mecsInitParamsQcmdpc(BPU_T_UN_Mecs_Params * params, const uint16_t m,
                             const uint16_t n0, const uint16_t w,
                             const uint16_t t) {
    return BPU_codeInitParamsQcmdpc((BPU_T_UN_Code_Params *) params, m, n0, w,
                                    t);
}

void BPU_mecsFreeParamsQcmdpc(BPU_T_UN_Mecs_Params * params) {
    BPU_codeFreeParamsQcmdpc((BPU_T_UN_Code_Params *) params);
}
