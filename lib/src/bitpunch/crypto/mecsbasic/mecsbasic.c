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
#include "mecsbasic.h"

#include <bitpunch/math/gf2.h>
#include <bitpunch/debugio.h>

int BPU_mecsBasicEncryptA(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const BPU_T_Mecs_Ctx *ctx) {
	if (BPU_gf2VecMalloc(out, ctx->code_ctx->code_len)) {
		BPU_printError("BPU_encryptA: can not allocate ouput");

		return 1;
	}
	return BPU_mecsBasicEncrypt(out, in, ctx);
}

int BPU_mecsBasicEncrypt(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const BPU_T_Mecs_Ctx *ctx) {
	int rc;
	BPU_T_GF2_Vector e;
	BPU_gf2VecNull(out);

	// test the size of message and g_m
	if (in->len != ctx->code_ctx->msg_len) {
		BPU_printError("message length have to be of length %d", ctx->code_ctx->msg_len);

		return -1;
	}
#ifdef BPU_DEBUG_ENCRYPT
	BPU_printDebug("plaintext:");
	BPU_printGf2Vec(message);
#endif
	rc = ctx->code_ctx->_encode(out, in, ctx->code_ctx);
	if (rc) {
		BPU_printError("BPU_mecsBasicEncrypt: can not encode");
		return rc;
	}
#ifdef BPU_DEBUG_ENCRYPT
	BPU_printDebug("encoded plaintext:");
	BPU_printGf2Vec(out);
#endif
	// generate random error vector e
	rc = BPU_gf2VecRand(&e, ctx->code_ctx->code_len, ctx->code_ctx->t);
	if (rc) {
		BPU_printError("BPU_mecsBasicEncrypt: can not init rand vector");
		return rc;
	}
#ifdef BPU_DEBUG_ENCRYPT
	BPU_printDebug("error vector:");
	BPU_printGf2Vec(&e);
#endif
	// z' XOR e
	rc = BPU_gf2VecXor(out, &e);
	if (rc) {
		BPU_printError("BPU_mecsBasicEncrypt: can not add error vector");
		return rc;
	}
#ifdef BPU_DEBUG_ENCRYPT
	BPU_printDebug("cipher text:");
	BPU_printGf2Vec(out);
	BPU_printDebug("###### ENCRYPT DBPU_ONE ######\n");
#endif
	BPU_gf2VecFree(&e, 0);

	return rc;
}

int BPU_mecsBasicDecrypt(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const BPU_T_Mecs_Ctx *ctx) {
	// Vector_GF2 tmp;
	int rc = BPU_mecsBasicDecryptA(out, in, ctx);

	// if (!rc) {
	//     BPU_gf2VecCopy(message, &tmp);
	//     BPU_freeVecGF2(&tmp, 0);
	// }
	return rc;
}

int BPU_mecsBasicDecryptA(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const BPU_T_Mecs_Ctx *ctx) {
	int rc = 0;
	BPU_T_GF2_Vector temp;

	BPU_gf2VecMalloc(&temp, in->len);
	BPU_gf2VecCopy(&temp, in);

	BPU_gf2VecMalloc(out, ctx->code_ctx->msg_len);
	ctx->code_ctx->_decode(out, &temp, ctx->code_ctx);

#ifdef BPU_DEBUG_DECRYPT
	BPU_printDebug("decoded error:");
	BPU_printGf2Vec(&decoded);
#endif
	BPU_gf2VecFree(&temp, 0);

#ifdef BPU_DEBUG_DECRYPT
	BPU_printDebug("###### DECRYPT DBPU_ONE ######");
#endif
	return rc;
}
