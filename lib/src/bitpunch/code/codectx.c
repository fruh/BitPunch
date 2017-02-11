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
#include <bitpunch/code/codectx.h>

#include <stdlib.h>

#include <bitpunch/debugio.h>
#include <bitpunch/errorcodes.h>
#include <bitpunch/math/gf2.h>

// additional codes
#include <bitpunch/code/goppa/goppa.h>
#include <bitpunch/code/qcmdpc/qcmdpc.h>

BPU_T_Code_Ctx *BPU_codeCtxNew(const BPU_T_UN_Code_Params * params,
                               const BPU_T_EN_Code_Types type)
{
    BPU_T_Code_Ctx *ctx_local = NULL;
    BPU_T_Code_Ctx *ctx = NULL;
    BPU_T_UN_Code_Spec *code_spec_local = NULL;
    BPU_T_Math_Ctx *math_ctx_local = NULL;
    BPU_T_Goppa_Spec *goppa_spec_local = NULL;

    if (NULL == params) {
        BPU_printError("Invalid input parameter \"%s\"", "params");
        goto err;
    }

    ctx_local = (BPU_T_Code_Ctx *) calloc(1, sizeof(BPU_T_Code_Ctx));
    if (NULL == ctx_local) {
        BPU_printError("calloc failed");
        goto err;
    }

    ctx_local->type = type;
    code_spec_local =
        (BPU_T_UN_Code_Spec *) calloc(1, sizeof(BPU_T_UN_Code_Spec));
    if (NULL == code_spec_local) {
        BPU_printError("Can not malloc BPU_T_UN_Code_Spec");
        goto err;
    }

    switch (type) {
    case BPU_EN_CODE_GOPPA:
#ifdef BPU_CONF_ENCRYPTION
        ctx_local->_encode = BPU_goppaEncode;
#endif
#ifdef BPU_CONF_DECRYPTION
        ctx_local->_decode = BPU_goppaDecode;
#endif
        math_ctx_local = BPU_codeMathCtxNew(params->goppa->m,
                                            params->goppa->mod);
        if (NULL == math_ctx_local) {
            BPU_printError("Code math context initialization ERROR.");
            goto err;
        }

        goppa_spec_local =
            (BPU_T_Goppa_Spec *) calloc(1, sizeof(BPU_T_Goppa_Spec));
        if (NULL == goppa_spec_local) {
            BPU_printError("Can not malloc BPU_T_Goppa_Spec");
            goto err;
        }

        goppa_spec_local->support_len = (1 << params->goppa->m); // ctx->math_ctx->ord + 1;
        ctx_local->code_len = goppa_spec_local->support_len;
        ctx_local->msg_len = goppa_spec_local->support_len - params->goppa->m * params->goppa->t; // n - m*t
        ctx_local->t = params->goppa->t;
        code_spec_local->goppa = goppa_spec_local;
        goppa_spec_local = NULL;
        ctx_local->code_spec = code_spec_local;
        code_spec_local = NULL;
        ctx_local->math_ctx = math_ctx_local;
        math_ctx_local = NULL;
        break;
    case BPU_EN_CODE_QCMDPC:
        ctx_local->code_spec->qcmdpc =
            (BPU_T_Qcmdpc_Spec *) calloc(1, sizeof(BPU_T_Qcmdpc_Spec));
#ifdef BPU_CONF_ENCRYPTION
        ctx_local->_encode = BPU_mecsQcmdpcEncode;
#endif
#ifdef BPU_CONF_DECRYPTION
        ctx_local->_decode = BPU_mecsQcmdpcDecrypt;
#endif
        if (!ctx_local->code_spec->qcmdpc) {
            BPU_printError("Can not malloc BPU_T_Goppa_Spec");
            goto err;
        }

        ctx_local->code_spec->qcmdpc->m = params->qcmdpc->m;
        ctx_local->code_spec->qcmdpc->n0 = params->qcmdpc->n0;
        ctx_local->code_spec->qcmdpc->w = params->qcmdpc->w;
        ctx_local->code_len = params->qcmdpc->m * params->qcmdpc->n0;
        ctx_local->msg_len = ctx_local->code_len - params->qcmdpc->m;
        ctx_local->t = params->qcmdpc->t;
        break;
        /* EXAMPLE please DO NOT REMOVE
           case BPU_EN_CODE_*****:
           #ifdef BPU_CONF_ENCRYPTION
           ctx_p->_encode = FUNC_FROM_YOUR_FILE;
           #endif
           #ifdef BPU_CONF_DECRYPTION
           ctx_p->_decode = FUNC_FROM_YOUR_FILE;
           #endif
           ctx->code_spec->YOURS = ALLOC OR NULL;

           ctx_p->code_len = LEN;
           ctx_p->msg_len = LEN;
           ctx_p->t = T;
           break;
         */
    default:
        BPU_printError("Code type not supported: %d", type);
        goto err;
    }

    ctx = ctx_local;
    ctx_local = NULL;
 err:
    BPU_SAFE_FREE(free, ctx_local);
    BPU_SAFE_FREE(BPU_mathCtxFree, math_ctx_local);
    BPU_SAFE_FREE(free, goppa_spec_local);
    BPU_SAFE_FREE(free, code_spec_local);
    return ctx;
}

BPU_T_Math_Ctx *BPU_codeMathCtxNew(const uint16_t m, const BPU_T_GF2_16x mod)
{
    BPU_T_Math_Ctx *math_ctx_local = NULL;
    BPU_T_Math_Ctx *math_ctx = NULL;
    BPU_T_GF2_16x context_mod = 0;

    if (mod != 0) {
        context_mod = mod;
    } else if (m == 5) {
        context_mod = BPU_GF2_POLY_DEG_5;
    } else if (m == 6) {
        context_mod = BPU_GF2_POLY_DEG_6;
    } else if (m == 6) {
        context_mod = BPU_GF2_POLY_DEG_6;
    } else if (m == 11) {
        context_mod = BPU_GF2_POLY_DEG_11;
    } else {
        BPU_printError
            ("Code params not supported. Supported only (m,t): (5,5), (6,6), (6,7), (11,50)");
        goto err;
    }

    math_ctx_local = BPU_mathCtxNew((BPU_T_GF2_16x) 2, context_mod);
    if (NULL == math_ctx_local) {
        BPU_printError("BPU_mathCtxNew failed");
        goto err;
    }

    math_ctx = math_ctx_local;
    math_ctx_local = NULL;
 err:
    BPU_SAFE_FREE(BPU_mathCtxFree, math_ctx_local);
    return math_ctx;
}

void BPU_codeCtxFree(BPU_T_Code_Ctx * ctx)
{
    if (NULL == ctx) {
        return;
    }

    switch (ctx->type) {
    case BPU_EN_CODE_GOPPA:
        BPU_goppaFreeSpec(ctx->code_spec->goppa);
        free(ctx->code_spec->goppa);
        break;
    case BPU_EN_CODE_QCMDPC:
        BPU_qcmdpcFreeSpec(ctx->code_spec->qcmdpc);
        break;
    default:
        BPU_printError("Code type not supported: %d", ctx->type);
    }

    BPU_mathCtxFree(ctx->math_ctx);

    free(ctx->code_spec);
    free(ctx);
}

BPU_T_UN_Code_Params *BPU_codeParamsGoppaNew(const uint16_t m,
                                             const uint16_t t,
                                             const BPU_T_GF2_16x mod)
{
    BPU_T_UN_Code_Params *code_params_local = NULL;
    BPU_T_UN_Code_Params *code_params = NULL;
    BPU_T_Goppa_Params *goppa_params = NULL;

    code_params_local =
        (BPU_T_UN_Code_Params *) calloc(1, sizeof(BPU_T_UN_Code_Params));
    if (NULL == code_params_local) {
        BPU_printError("calloc failed");
        goto err;
    }

    goppa_params = BPU_goppaParamsNew(m, t, mod);
    if (NULL == goppa_params) {
        BPU_printError("BPU_goppaParamsNew failed");
        goto err;
    }

    code_params_local->goppa = goppa_params;
    goppa_params = NULL;

    code_params = code_params_local;
    code_params_local = NULL;
 err:
    BPU_SAFE_FREE(BPU_goppaFreeParams, goppa_params);
    BPU_SAFE_FREE(free, code_params_local);
    return code_params;
}

void BPU_codeFreeParamsGoppa(BPU_T_UN_Code_Params * params)
{
    BPU_SAFE_FREE(BPU_goppaFreeParams, params->goppa);
    BPU_SAFE_FREE(free, params);
}

BPU_T_UN_Code_Params * BPU_codeInitParamsQcmdpc(const uint16_t m,
                             const uint16_t n0, const uint16_t w,
                             const uint16_t t)
{
    BPU_T_UN_Code_Params *code_params_local = NULL;
    BPU_T_UN_Code_Params *code_params = NULL;
    BPU_T_Qcmdpc_Params *qcmdpc_params = NULL;

    code_params_local =
        (BPU_T_UN_Code_Params *) calloc(1, sizeof(BPU_T_UN_Code_Params));
    if (NULL == code_params_local) {
        BPU_printError("calloc failed");
        goto err;
    }

    qcmdpc_params = BPU_qcmdpcInitParams(m, n0, w, t);
    if (NULL == qcmdpc_params) {
        BPU_printError("BPU_qcmdpcInitParams failed");
        goto err;
    }

    code_params_local->qcmdpc = qcmdpc_params;
    qcmdpc_params = NULL;

    code_params = code_params_local;
    code_params_local = NULL;
 err:
    BPU_SAFE_FREE(BPU_qcmdpcFreeParams, qcmdpc_params);
    BPU_SAFE_FREE(free, code_params_local);
    return code_params;
}

void BPU_codeFreeParamsQcmdpc(BPU_T_UN_Code_Params * params)
{
    BPU_SAFE_FREE(BPU_qcmdpcFreeParams, params->qcmdpc);
    BPU_SAFE_FREE(free, params);
}
