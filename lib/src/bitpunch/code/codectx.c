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
#include "codectx.h"

#include <stdlib.h>

#include <bitpunch/debugio.h>
#include <bitpunch/errorcodes.h>
#include <bitpunch/math/gf2.h>

// additional codes
#include <bitpunch/code/goppa/goppa.h>
#include <bitpunch/code/qcmdpc/qcmdpc.h>
#include <bitpunch/code/qcmdpc/qcmdpctypes.h>

int BPU_codeInitCtx(BPU_T_Code_Ctx ** ctx, const BPU_T_UN_Code_Params * params,
                    const BPU_T_EN_Code_Types type) {
    int tmp;
    BPU_T_Code_Ctx *ctx_p;

    *ctx = (BPU_T_Code_Ctx *) calloc(1, sizeof(BPU_T_Code_Ctx));
    if (!*ctx) {
        BPU_printError("Can not malloc BPU_T_Code_Ctx");

        return BPU_EC_MALLOC_ERROR;
    }
    ctx_p = *ctx;
    ctx_p->type = type;

    ctx_p->code_spec =
        (BPU_T_UN_Code_Spec *) calloc(1, sizeof(BPU_T_UN_Code_Spec));
    if (!ctx_p->code_spec) {
        BPU_printError("Can not malloc BPU_T_UN_Code_Spec");

        return BPU_EC_MALLOC_ERROR;
    }
    switch (type) {
    case BPU_EN_CODE_GOPPA:
#ifdef BPU_CONF_ENCRYPTION
        ctx_p->_encode = BPU_goppaEncode;
#endif
#ifdef BPU_CONF_DECRYPTION
        ctx_p->_decode = BPU_goppaDecode;
#endif
        tmp =
            BPU_codeInitMathCtx(&ctx_p->math_ctx, params->goppa->m,
                                params->goppa->t, params->goppa->mod);
        if (tmp) {
            BPU_printError("Code math context initialization ERROR.");

            return tmp;
        }
        ctx_p->code_spec->goppa =
            (BPU_T_Goppa_Spec *) calloc(1, sizeof(BPU_T_Goppa_Spec));
        if (!ctx_p->code_spec->goppa) {
            BPU_printError("Can not malloc BPU_T_Goppa_Spec");

            return BPU_EC_MALLOC_ERROR;
        }
        ctx_p->code_spec->goppa->support_len = (1 << params->goppa->m); // ctx->math_ctx->ord + 1;
        ctx_p->code_len = ctx_p->code_spec->goppa->support_len;
        ctx_p->msg_len = ctx_p->code_spec->goppa->support_len - params->goppa->m * params->goppa->t;    // n - m*t
        ctx_p->t = params->goppa->t;

        break;
    case BPU_EN_CODE_QCMDPC:
        ctx_p->code_spec->qcmdpc =
            (BPU_T_Qcmdpc_Spec *) calloc(1, sizeof(BPU_T_Qcmdpc_Spec));
#ifdef BPU_CONF_ENCRYPTION
        ctx_p->_encode = BPU_mecsQcmdpcEncode;
        // ctx_p->code_spec->qcmdpc->G = (BPU_T_GF2_QC_Matrix *) malloc(sizeof(BPU_T_GF2_QC_Matrix));
#endif
#ifdef BPU_CONF_DECRYPTION
        ctx_p->_decode = BPU_mecsQcmdpcDecrypt;
        // ctx_p->code_spec->qcmdpc->H = (BPU_T_GF2_Sparse_Qc_Matrix *) malloc(sizeof(BPU_T_GF2_Sparse_Qc_Matrix));
#endif
        if (!ctx_p->code_spec->qcmdpc) {
            BPU_printError("Can not malloc BPU_T_Goppa_Spec");

            return BPU_EC_MALLOC_ERROR;
        }
        ctx_p->code_spec->qcmdpc->m = params->qcmdpc->m;
        ctx_p->code_spec->qcmdpc->n0 = params->qcmdpc->n0;
        ctx_p->code_spec->qcmdpc->w = params->qcmdpc->w;
        ctx_p->code_len = params->qcmdpc->m * params->qcmdpc->n0;
        ctx_p->msg_len = ctx_p->code_len - params->qcmdpc->m;
        ctx_p->t = params->qcmdpc->t;

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
        return BPU_EC_CODE_TYPE_NOT_SUPPORTED;
    }
    // error vector
    if (BPU_gf2VecMalloc(&ctx_p->e, ctx_p->code_len)) {
        BPU_printError("can not allocate error vector");
        return BPU_EC_MALLOC_ERROR;
    }
    return 0;
}

int BPU_codeInitMathCtx(BPU_T_Math_Ctx ** ctx, const uint16_t m,
                        const uint16_t t, const BPU_T_GF2_16x mod) {
    int rc = 0;

    if (mod == (BPU_T_GF2_16x) - 1) {
        *ctx = (BPU_T_Math_Ctx *) calloc(1, sizeof(BPU_T_Math_Ctx));
        if (!*ctx) {
            BPU_printError("Can not malloc BPU_T_Math_Ctx");

            return -1;
        }
        (*ctx)->mod_deg = m;
    }
    else if (mod != 0) {
        rc = BPU_mathInitCtx(ctx, (BPU_T_GF2_16x) 2, mod);
    }
    else if (m == 5 && t == 5) {
        rc = BPU_mathInitCtx(ctx, (BPU_T_GF2_16x) 2,
                             (BPU_T_GF2_16x) BPU_GF2_POLY_DEG_5);
    }
    else if (m == 6 && t == 6) {
        rc = BPU_mathInitCtx(ctx, (BPU_T_GF2_16x) 2,
                             (BPU_T_GF2_16x) BPU_GF2_POLY_DEG_6);
    }
    else if (m == 6 && t == 7) {
        rc = BPU_mathInitCtx(ctx, (BPU_T_GF2_16x) 2,
                             (BPU_T_GF2_16x) BPU_GF2_POLY_DEG_6);
    }
    else if (m == 11 && t == 50) {
        rc = BPU_mathInitCtx(ctx, (BPU_T_GF2_16x) 2,
                             (BPU_T_GF2_16x) BPU_GF2_POLY_DEG_11);
    }
    else {
        BPU_printError
            ("Code params not supported. Supported only (m,t): (5,5), (6,6), (6,7), (11,50)");

        return BPU_EC_CODE_PARAMS_NOT_SUPPORTED;
    }
    return rc;
}

void BPU_codeFreeCtx(BPU_T_Code_Ctx ** ctx) {
    BPU_T_Code_Ctx *ctx_p = *ctx;

    if (!ctx_p) {
        return;
    }
    switch (ctx_p->type) {
    case BPU_EN_CODE_GOPPA:
        BPU_goppaFreeSpec(ctx_p->code_spec->goppa);
        free(ctx_p->code_spec->goppa);
        break;
    case BPU_EN_CODE_QCMDPC:
        BPU_qcmdpcFreeSpec(ctx_p->code_spec->qcmdpc);
        // free(ctx_p->code_spec->qcmdpc);
        break;
    default:
        BPU_printError("Code type not supported: %d", ctx_p->type);
    }
    BPU_gf2VecFree(&ctx_p->e);
    BPU_mathFreeCtx(&ctx_p->math_ctx);

    free(ctx_p->code_spec);
    free(ctx_p);
    *ctx = NULL;
}

int BPU_codeInitParamsGoppa(BPU_T_UN_Code_Params * params, const uint16_t m,
                            const uint16_t t, const BPU_T_GF2_16x mod) {
    return BPU_goppaInitParams(&params->goppa, m, t, mod);
}

void BPU_codeFreeParamsGoppa(BPU_T_UN_Code_Params * params) {
    BPU_goppaFreeParams(&params->goppa);
}

int BPU_codeInitParamsQcmdpc(BPU_T_UN_Code_Params * params, const uint16_t m,
                             const uint16_t n0, const uint16_t w,
                             const uint16_t t) {
    return BPU_qcmdpcInitParams(&params->qcmdpc, m, n0, w, t);
}

void BPU_codeFreeParamsQcmdpc(BPU_T_UN_Code_Params * params) {
    BPU_qcmdpcFreeParams(&params->qcmdpc);
}
