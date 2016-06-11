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
#include <bitpunch/bitpunch.h>
#include <bitpunch/crypto/mecsctx.h>
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

BPU_T_Mecs_Ctx* BPU_mecsCtxNew(const BPU_T_UN_Mecs_Params * params,
                    const BPU_T_EN_Mecs_Types type) {
    BPU_T_Mecs_Ctx *ctx = NULL;
    BPU_T_Mecs_Ctx *ctx_local = NULL;
    BPU_T_Code_Ctx *code_ctx = NULL;

    if (NULL == params) {
        BPU_printError("Invalid input parameter \"%s\"", "params");
        goto err;
    }

    ctx_local = (BPU_T_Mecs_Ctx *) calloc(1, sizeof(BPU_T_Mecs_Ctx));
    if (NULL == ctx_local) {
        BPU_printError("calloc failed");
        goto err;
    }

    ctx_local->type = type;

    switch (type) {
    case BPU_EN_MECS_BASIC_GOPPA:
#ifdef BPU_CONF_ENCRYPTION
        ctx_local->_encrypt = BPU_mecsBasicEncrypt;
#endif
#ifdef BPU_CONF_DECRYPTION
        ctx_local->_decrypt = BPU_mecsBasicDecrypt;
#endif
#ifdef BPU_CONF_KEY_GEN
        ctx_local->_genKeyPair = BPU_goppaGenCode;
#endif
        code_ctx = BPU_codeCtxNew((BPU_T_UN_Code_Params *) params,
                                     BPU_EN_CODE_GOPPA);
        if (NULL == code_ctx) {
            BPU_printError("BPU_codeInitCtx failed");
            goto err;
        }

        ctx_local->pt_len = code_ctx->msg_len;
        ctx_local->ct_len = code_ctx->code_len;
        break;

#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
    case BPU_EN_MECS_CCA2_POINTCHEVAL_GOPPA:
#ifdef BPU_CONF_ENCRYPTION
        ctx_local->_encrypt = BPU_mecsPointchevalCCA2Encrypt;
#endif
#ifdef BPU_CONF_DECRYPTION
        ctx_local->_decrypt = BPU_mecsPointchevalCCA2Decrypt;
#endif
#ifdef BPU_CONF_KEY_GEN
        ctx_local->_genKeyPair = BPU_goppaGenCode;
#endif
        code_ctx = BPU_codeCtxNew((BPU_T_UN_Code_Params *) params,
                                     BPU_EN_CODE_GOPPA);
        if (NULL == code_ctx) {
            BPU_printError("BPU_codeInitCtx failed");
            goto err;
        }

        ctx_local->pt_len =
            BPU_HASH_LEN * 8 <
            code_ctx->msg_len ? BPU_HASH_LEN *
            8 : code_ctx->msg_len;
        ctx_local->ct_len = code_ctx->code_len + 2 * ctx_local->ct_len;
        break;
#endif

    case BPU_EN_MECS_BASIC_QCMDPC:
#ifdef BPU_CONF_ENCRYPTION
        ctx_local->_encrypt = BPU_mecsBasicEncrypt;
#endif
#ifdef BPU_CONF_DECRYPTION
        ctx_local->_decrypt = BPU_mecsBasicDecrypt;
#endif
#ifdef BPU_CONF_KEY_GEN
        ctx_local->_genKeyPair = BPU_mecsQcmdpcGenKeys;
#endif
        code_ctx = BPU_codeCtxNew((BPU_T_UN_Code_Params *) params,
                                     BPU_EN_CODE_QCMDPC);
        if (NULL == code_ctx) {
            BPU_printError("BPU_codeInitCtx failed");
            goto err;
        }

        ctx_local->pt_len = code_ctx->msg_len;
        ctx_local->ct_len = code_ctx->code_len;
        break;

#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_QCMDPC
    case BPU_EN_MECS_CCA2_POINTCHEVAL_QCMDPC:
#ifdef BPU_CONF_ENCRYPTION
        ctx_local->_encrypt = BPU_mecsPointchevalCCA2Encrypt;
#endif
#ifdef BPU_CONF_DECRYPTION
        ctx_local->_decrypt = BPU_mecsPointchevalCCA2Decrypt;
#endif
#ifdef BPU_CONF_KEY_GEN
        ctx_local->_genKeyPair = BPU_mecsQcmdpcGenKeys;
#endif
        code_ctx = BPU_codeCtxNew((BPU_T_UN_Code_Params *) params,
                                     BPU_EN_CODE_QCMDPC);
        if (NULL == code_ctx) {
            BPU_printError("BPU_codeInitCtx failed");
            goto err;
        }

        ctx_local->pt_len =
            BPU_HASH_LEN * 8 <
            code_ctx->msg_len ? BPU_HASH_LEN *
            8 : code_ctx->msg_len;
        // TODO: critical
        ctx_local->ct_len = code_ctx->code_len + 2 * ctx_local->ct_len;
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
           ctx_p->code_ctx = BPU_codeInitCtx(m, t, BPU_EN_CODE_GOPPA, mod);
           if (ctx_p->code_ctx) {
           goto err;
           }
           ctx->pt_len = PT_LEN;
           ctx->ct_len = CT_LEN;
           break;
         */
    default:
        BPU_printError("MECS type not supported: %d", type);
        goto err;
    }

    ctx_local->code_ctx = code_ctx;
    ctx = ctx_local;
    ctx_local = NULL;
    code_ctx = NULL;
err:
    // TODO: in case of error code context is not released
    BPU_SAFE_FREE(free, ctx_local);
    BPU_SAFE_FREE(BPU_codeCtxFree, code_ctx);
    return ctx;
}

void BPU_mecsFreeCtx(BPU_T_Mecs_Ctx *ctx) {
    if (NULL == ctx) {
        return;
    }

    switch (ctx->type) {
    case BPU_EN_MECS_BASIC_GOPPA:
#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
    case BPU_EN_MECS_CCA2_POINTCHEVAL_GOPPA:
#endif
    case BPU_EN_MECS_BASIC_QCMDPC:
#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_QCMDPC
    case BPU_EN_MECS_CCA2_POINTCHEVAL_QCMDPC:
#endif
        ctx->_encrypt = NULL;
        ctx->_decrypt = NULL;
        ctx->_genKeyPair = NULL;
        break;
    default:
        BPU_printError("MECS type not supported: %d", ctx->type);
    }

    BPU_SAFE_FREE(BPU_codeCtxFree, ctx->code_ctx);
    BPU_SAFE_FREE(free, ctx);
}

BPU_T_UN_Mecs_Params * BPU_mecsParamsGoppaNew(const uint16_t m,
                            const uint16_t t, const BPU_T_GF2_16x mod) {
    BPU_T_UN_Mecs_Params *mecs_params_local = NULL;
    BPU_T_UN_Mecs_Params *mecs_params = NULL;

    mecs_params_local = BPU_codeParamsGoppaNew(m, t, mod);
    if (NULL == mecs_params_local) {
        BPU_printError("code_params_local failed");
        goto err;
    }

    mecs_params = mecs_params_local;
    mecs_params_local = NULL;
err:
    BPU_SAFE_FREE(BPU_codeFreeParamsGoppa, mecs_params_local);
    return mecs_params;
}

void BPU_mecsDestroyParamsGoppa(BPU_T_UN_Mecs_Params * params) {
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
