/**
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
#include "codectx.h"

#include <stdlib.h>

#include <bitpunch/debugio.h>
#include <bitpunch/errorcodes.h>

// addiotional codes
#include <bitpunch/code/goppa/goppa.h>

int BPU_codeInitCtx(BPU_T_Code_Ctx *ctx, const uint16_t m, const uint16_t t, const BPU_T_EN_Code_Types type) {
	int tmp;
	ctx->type = type;

	ctx->math_ctx = (BPU_T_Math_Ctx *) calloc(1, sizeof(BPU_T_Math_Ctx));
	if (!ctx->math_ctx) {
		BPU_printError("Can not malloc BPU_T_Math_Ctx");

		return BPU_EC_MALLOC_ERROR;
	}
	ctx->code_spec = calloc(1, sizeof(BPU_T_UN_Code_Spec));
	if (!ctx->code_spec) {
		BPU_printError("Can not malloc BPU_T_UN_Code_Spec");
		free(ctx->math_ctx);

		return BPU_EC_MALLOC_ERROR;
	}
	tmp = BPU_codeInitMathCtx(ctx->math_ctx, m, t);
	if (tmp) {
		BPU_printError("Code math context initialization ERROR.");

		return tmp;
	}

	switch (type) {
	case BPU_EN_CODE_GOPPA:
		ctx->_encode = BPU_goppaEncodeM;
		ctx->_decode = BPU_goppaDecode;
		ctx->code_len = ctx->math_ctx->ord; // ord
		ctx->msg_len = ctx->math_ctx->ord - m*t; // n - m*t
		ctx->t = t;
		ctx->code_spec->goppa = (BPU_T_Goppa_Spec *) calloc(1, sizeof(BPU_T_Goppa_Spec));

		if (!ctx->code_spec->goppa) {
			BPU_printError("Can not malloc BPU_T_Goppa_Spec");

			return BPU_EC_MALLOC_ERROR;
		}
		break;
	/* EXAMPLE please DO NOT REMOVE
	case BPU_EN_CODE_*****:
		ctx->_encode = FUNC_FROM_YOUR_FILE;
		ctx->_decode = FUNC_FROM_YOUR_FILE;
		ctx->code_spec->YOURS = ALLOC OR NULL;
		break;
		*/
	default:
		free(ctx->math_ctx);

		BPU_printError("Code type not supported: %d", type);
		return BPU_EC_CODE_TYPE_NOT_SUPPORTED;
	}
	return 0;
}

int BPU_codeInitMathCtx(BPU_T_Math_Ctx *ctx, const uint16_t m, const uint16_t t) {
	int rc;

	if (m == 5 && t == 5) {
		rc = BPU_mathInitCtx(ctx, (BPU_T_GF2_16x)2, (BPU_T_GF2_16x) BPU_GF2_POLY_DEG_5);
	}
	else if (m == 6 && t == 6) {
		rc = BPU_mathInitCtx(ctx, (BPU_T_GF2_16x)2, (BPU_T_GF2_16x) BPU_GF2_POLY_DEG_6);
	}
	else if (m == 6 && t == 7) {
		rc = BPU_mathInitCtx(ctx, (BPU_T_GF2_16x)2, (BPU_T_GF2_16x) BPU_GF2_POLY_DEG_6);
	}
	else if (m == 11 && t == 50) {
		rc = BPU_mathInitCtx(ctx, (BPU_T_GF2_16x)2, (BPU_T_GF2_16x) BPU_GF2_POLY_DEG_11);
	}
	else {
		BPU_printError("Code params not supported. Supported only (m,t): (5,5), (6,6), (6,7), (11,50)");

		return BPU_EC_CODE_PARAMS_NOT_SUPPORTED;
	}
	return rc;
}

void BPU_codeFreeCtx(BPU_T_Code_Ctx *ctx) {
	switch (ctx->type) {
	case BPU_EN_CODE_GOPPA:
		BPU_goppaFreeSpec(ctx->code_spec->goppa);
		free(ctx->code_spec->goppa);
		break;
	default:
		BPU_printError("Code type not supported: %d", ctx->type);
	}
	BPU_mathFreeCtx(ctx->math_ctx, 0);
	free(ctx->math_ctx);
	free(ctx->code_spec);
}
