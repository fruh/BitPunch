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
#include "mecsctx.h"

#include <stdlib.h>

#include <bitpunch/debugio.h>
#include <bitpunch/errorcodes.h>
#include <bitpunch/code/codectx.h>
#include <bitpunch/math/mathctx.h>

// additional encryption schemes
#include <bitpunch/crypto/mecsbasic/mecsbasic.h>
#include <bitpunch/code/goppa/goppa.h>

#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
#include <bitpunch/crypto/cca2/mecspointcheval.h>
#endif

int BPU_mecsInitCtx(BPU_T_Mecs_Ctx *ctx, const uint16_t m, const uint16_t t, const BPU_T_EN_Mecs_Types type) {
	ctx->type = type;

	ctx->code_ctx = (BPU_T_Code_Ctx *) calloc(1, sizeof(BPU_T_Code_Ctx));
	if (!ctx->code_ctx) {
		BPU_printError("Can not malloc BPU_T_Code_Ctx");

		return BPU_EC_MALLOC_ERROR;
	}
	switch (type) {
	case BPU_EN_MECS_BASIC_GOPPA:
		ctx->_encrypt = BPU_mecsBasicEncrypt;
		ctx->_decrypt = BPU_mecsBasicDecrypt;
		ctx->_genKeyPair = BPU_goppaGenCode;

		BPU_codeInitCtx(ctx->code_ctx, m, t, BPU_EN_CODE_GOPPA);

		ctx->pt_len = ctx->code_ctx->msg_len;
		ctx->ct_len = ctx->code_ctx->code_len;
		break;

#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
	case BPU_EN_MECS_CCA2_POINTCHEVAL_GOPPA:
		ctx->_encrypt = BPU_mecsPointchevalCCA2Encrypt;
		ctx->_decrypt = BPU_mecsPointchevalCCA2Decrypt;
		ctx->_genKeyPair = BPU_goppaGenCode;

		BPU_codeInitCtx(ctx->code_ctx, m, t, BPU_EN_CODE_GOPPA);

		ctx->pt_len = BPU_HASH_LEN * 8 < ctx->code_ctx->msg_len ? BPU_HASH_LEN * 8 : ctx->code_ctx->msg_len;
		ctx->ct_len = ctx->code_ctx->code_len + 2 * ctx->ct_len;
		break;
#endif
	/* EXAMPLE please DO NOT REMOVE
	case BPU_EN_MECS_*****:
		ctx->_encrypt = FUNC_FROM_YOUR_FILE;
		ctx->_decrypt = FUNC_FROM_YOUR_FILE;
		ctx->_genKeyPair = FUNC_FROM_YOUR_FILE;

		BPU_codeInitCtx(ctx->code_ctx, BPU_EN_CODE_GOPPA);

		ctx->pt_len = PT_LEN;
		ctx->ct_len = CT_LEN;
		break;
		*/
	default:
		free(ctx->code_ctx);

		BPU_printError("MECS type not supported: %d", type);
		return BPU_EC_MECS_TYPE_NOT_SUPPORTED;
	}
	return 0;
}

int BPU_mecsFreeCtx(BPU_T_Mecs_Ctx *ctx) {
	switch (ctx->type) {
	case BPU_EN_MECS_BASIC_GOPPA:
#ifdef BPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA
	case BPU_EN_MECS_CCA2_POINTCHEVAL_GOPPA:
#endif
		ctx->_encrypt = NULL;
		ctx->_decrypt = NULL;
		ctx->_genKeyPair = NULL;
		break;
	default:
		BPU_printError("MECS type not supported: %d", ctx->type);
		return BPU_EC_MECS_TYPE_NOT_SUPPORTED;
	}
	BPU_codeFreeCtx(ctx->code_ctx);
	free(ctx->code_ctx);
	return 0;
}
